#define CUTE_TILED_IMPLEMENTATION
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// --- Texture Cache cho Bản đồ ---
#define MAX_MAP_CACHE 128
static struct {
    char path[256];
    Texture2D texture;
} MapTextureCache[MAX_MAP_CACHE];
static int MapCacheCount = 0;

static Texture2D GetCachedTexture(const char* path) {
    char correctedPath[512];
    strncpy(correctedPath, path, 511);
    correctedPath[511] = '\0';
    
    // Check if file exists in root. If not, try assets/ prefix
    if (!FileExists(correctedPath)) {
        snprintf(correctedPath, sizeof(correctedPath), "assets/%s", path);
    }

    for (int i = 0; i < MapCacheCount; i++) {
        if (strcmp(MapTextureCache[i].path, correctedPath) == 0) return MapTextureCache[i].texture;
    }
    if (MapCacheCount < MAX_MAP_CACHE) {
        Texture2D tex = LoadTexture(correctedPath);
        if (tex.id != 0) {
            strncpy(MapTextureCache[MapCacheCount].path, correctedPath, 255);
            MapTextureCache[MapCacheCount].texture = tex;
            MapCacheCount++;
        }
        return tex;
    }
    return LoadTexture(correctedPath);
}

static void ClearMapCache() {
    for (int i = 0; i < MapCacheCount; i++) UnloadTexture(MapTextureCache[i].texture);
    MapCacheCount = 0;
}

// --- Helpers ---
void MapFixPath(char* path) {
    char* p = path;
    // Chuyển \/ thành /
    while ((p = strstr(p, "\\/"))) {
        memmove(p, p + 1, strlen(p));
    }
    // Lấy phần sau "Theforest/" nếu có
    char* key = strstr(path, "Theforest/");
    if (key) {
        memmove(path, key + 10, strlen(key + 10) + 1);
    }
}

static cute_tiled_tileset_t* FindTileset(cute_tiled_map_t* map, int gid) {
    cute_tiled_tileset_t* ts = map->tilesets;
    cute_tiled_tileset_t* best = NULL;
    while (ts) {
        if (gid >= ts->firstgid) {
            if (!best || ts->firstgid > best->firstgid) best = ts;
        }
        ts = ts->next;
    }
    return best;
}

// --- API Implementation ---

cute_tiled_map_t* MapLoad(const char* filename) {
    cute_tiled_map_t* map = cute_tiled_load_map_from_file(filename, NULL);
    if (!map) printf("Error loading map %s: %s\n", filename, cute_tiled_error_reason);
    return map;
}

void MapUnload(cute_tiled_map_t* map) {
    ClearMapCache();
    cute_tiled_free_map(map);
}

void MapDrawLayer(cute_tiled_map_t* map, const char* layerName, float offsetX, Texture2D defaultTileset) {
    if (!map) return;
    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        if (strcmp(layer->name.ptr, layerName) == 0 && layer->visible) {
            // 1. TILE LAYER
            if (strcmp(layer->type.ptr, "tilelayer") == 0) {
                for (int i = 0; i < layer->data_count; i++) {
                    int rawGid = layer->data[i];
                    int gid = cute_tiled_unset_flags(rawGid);
                    if (gid == 0) continue;

                    cute_tiled_tileset_t* ts = FindTileset(map, gid);
                    if (!ts) continue;

                    int tileId = gid - ts->firstgid;
                    int tsCols = ts->columns;
                    int tw = ts->tilewidth;
                    int th = ts->tileheight;

                    Texture2D currentTex = defaultTileset;
                    bool isIndividual = (tsCols <= 0);

                    if (isIndividual) {
                        cute_tiled_tile_descriptor_t* td = ts->tiles;
                        while (td) {
                            if (td->tile_index == tileId) {
                                if (td->image.ptr && strlen(td->image.ptr) > 0) {
                                    char tsPath[256];
                                    strncpy(tsPath, td->image.ptr, 255);
                                    MapFixPath(tsPath);
                                    currentTex = GetCachedTexture(tsPath);
                                }
                                break;
                            }
                            td = td->next;
                        }
                    } else if (ts->image.ptr && strlen(ts->image.ptr) > 0) {
                        char tsPath[256];
                        strncpy(tsPath, ts->image.ptr, 255);
                        MapFixPath(tsPath);
                        currentTex = GetCachedTexture(tsPath);
                    }

                    Rectangle source;
                    if (isIndividual) {
                        source = (Rectangle){ 0, 0, (float)currentTex.width, (float)currentTex.height };
                    } else {
                        source = (Rectangle){ (float)(tileId % tsCols) * tw, (float)(tileId / tsCols) * th, (float)tw, (float)th };
                    }

                    float tileW = isIndividual ? (float)currentTex.width : (float)tw;
                    float tileH = isIndividual ? (float)currentTex.height : (float)th;
                    
                    int x = i % layer->width;
                    int y = i / layer->width;
                    
                    float posX = (float)x * map->tilewidth + offsetX;
                    float posY = (float)y * map->tileheight + map->tileheight - tileH;
                    
                    // Xử lý các cờ lật (flip flags) từ Tiled
                    bool flipH = (rawGid & CUTE_TILED_FLIPPED_HORIZONTALLY_FLAG) != 0;
                    bool flipV = (rawGid & CUTE_TILED_FLIPPED_VERTICALLY_FLAG) != 0;
                    bool flipD = (rawGid & CUTE_TILED_FLIPPED_DIAGONALLY_FLAG) != 0;
                    
                    float rotation = 0.0f;
                    Rectangle srcRec = source;
                    
                    if (flipD) {
                        rotation = 90.0f;
                        bool temp = flipH;
                        flipH = !flipV;
                        flipV = temp;
                    }
                    
                    if (flipH) srcRec.width = -srcRec.width;
                    if (flipV) srcRec.height = -srcRec.height;
                    
                    Rectangle destRec = { posX + tileW/2.0f, posY + tileH/2.0f, tileW, tileH };
                    Vector2 origin = { tileW/2.0f, tileH/2.0f };
                    DrawTexturePro(currentTex, srcRec, destRec, origin, rotation, Fade(WHITE, layer->opacity));
                }
            }
            // 2. OBJECT LAYER
            else if (strcmp(layer->type.ptr, "objectgroup") == 0) {
                cute_tiled_object_t* obj = layer->objects;
                while (obj) {
                    if (obj->visible && obj->gid != 0) {
                        int rawGid = obj->gid;
                        int gid = cute_tiled_unset_flags(rawGid);
                        cute_tiled_tileset_t* ts = FindTileset(map, gid);
                        if (ts) {
                            cute_tiled_tile_descriptor_t* td = ts->tiles;
                            while (td) {
                                if (td->tile_index == (gid - ts->firstgid)) {
                                    char path[256];
                                    strncpy(path, td->image.ptr, 255);
                                    MapFixPath(path);
                                    Texture2D tex = GetCachedTexture(path);
                                    if (tex.id != 0) {
                                        Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
                                        if (rawGid & CUTE_TILED_FLIPPED_HORIZONTALLY_FLAG) source.width = -source.width;
                                        Rectangle dest = { obj->x + offsetX, obj->y, (float)obj->width, (float)obj->height };
                                        DrawTexturePro(tex, source, dest, (Vector2){0, (float)obj->height}, obj->rotation, Fade(WHITE, layer->opacity));
                                    }
                                    break;
                                }
                                td = td->next;
                            }
                        }
                    }
                    obj = obj->next;
                }
            }
            break;
        }
        layer = layer->next;
    }
}

float MapGetGroundY(cute_tiled_map_t* map, float x, float currentY) {
    if (!map) return 0;
    cute_tiled_layer_t* l = map->layers;
    while (l) {
        if (strcmp(l->name.ptr, "ground") == 0 && l->objects) {
            cute_tiled_object_t* obj = l->objects;
            
            float candidates[128];
            int count = 0;
            
            while (obj) {
                float objBestY = -1.0f;
                if (obj->vert_count > 0) {
                    for (int i = 0; i < obj->vert_count; i++) {
                        int next = (i + 1) % obj->vert_count;
                        float x1 = obj->x + obj->vertices[i * 2];
                        float y1 = obj->y + obj->vertices[i * 2 + 1];
                        float x2 = obj->x + obj->vertices[next * 2];
                        float y2 = obj->y + obj->vertices[next * 2 + 1];
                        
                        if (x1 == x2) continue; // Bỏ qua đoạn thẳng đứng
                        
                        if ((x1 <= x && x2 >= x) || (x2 <= x && x1 >= x)) {
                            float t = (x - x1) / (x2 - x1);
                            float y_at_x = y1 + t * (y2 - y1);
                            if (objBestY == -1.0f || y_at_x < objBestY) {
                                objBestY = y_at_x;
                            }
                        }
                    }
                } else if (obj->width > 0) {
                    float x1 = obj->x;
                    float x2 = obj->x + obj->width;
                    if (x >= x1 && x <= x2) {
                        objBestY = obj->y;
                    }
                } else {
                    objBestY = obj->y;
                }
                
                if (objBestY != -1.0f) {
                    if (count < 128) {
                        candidates[count++] = objBestY;
                    }
                }
                obj = obj->next;
            }
            
            if (count == 0) {
                return l->objects->y; // Trả về đối tượng đầu tiên nếu không tìm thấy gì khác
            }
            
            // Nếu không xác định currentY (như khởi tạo), chọn nền cao nhất (Y nhỏ nhất) bỏ qua trần
            if (currentY == -9999.0f) {
                float bestY = -1.0f;
                for (int i = 0; i < count; i++) {
                    if (candidates[i] >= 150.0f) {
                        if (bestY == -1.0f || candidates[i] > bestY) {
                            bestY = candidates[i];
                        }
                    }
                }
                if (bestY == -1.0f) {
                    bestY = candidates[0]; // fallback
                }
                return bestY;
            }
            
            // Chọn nền phù hợp nhất với Y hiện tại của mèo:
            // Lấy mặt đất cao nhất nằm dưới chân mèo (candidates[i] >= currentY - tolerance)
            float bestY = -1.0f;
            float tolerance = 8.0f; // Tolerance khoảng 1/2 tile height để tránh hút lên trần khi nhảy từ dưới
            
            for (int i = 0; i < count; i++) {
                if (candidates[i] < 150.0f) continue; // Bỏ qua trần nhà
                
                if (candidates[i] >= currentY - tolerance) {
                    if (bestY == -1.0f || candidates[i] < bestY) {
                        bestY = candidates[i];
                    }
                }
            }
            
            if (bestY != -1.0f) {
                return bestY;
            }
            
            // Nếu nằm hoàn toàn dưới tất cả mặt đất, lấy mặt đất gần nhất (bỏ qua trần)
            bestY = -1.0f;
            float minDist = -1.0f;
            for (int i = 0; i < count; i++) {
                if (candidates[i] < 150.0f) continue; // Bỏ qua trần nhà
                float dist = fabsf(candidates[i] - currentY);
                if (bestY == -1.0f || dist < minDist) {
                    minDist = dist;
                    bestY = candidates[i];
                }
            }
            if (bestY != -1.0f) {
                return bestY;
            }
            
            return candidates[0];
        }
        l = l->next;
    }
    return 642.0f; // Mặc định dự phòng
}

Vector2 MapGetSpawnPoint(cute_tiled_map_t* map) {
    if (!map) return (Vector2){100, 448};
    cute_tiled_layer_t* l = map->layers;
    while (l) {
        if ((strcmp(l->name.ptr, "spawn") == 0 || strcmp(l->name.ptr, "spawnpoint") == 0) && l->objects) {
            float spawnX = l->objects->x;
            float spawnY = l->objects->y;
            if (l->objects->width > 0) spawnX += l->objects->width / 2.0f;
            if (l->objects->height > 0) spawnY += l->objects->height;
            return (Vector2){ spawnX, spawnY };
        }
        l = l->next;
    }
    // Fallback nếu không có spawn layer: tìm nền đất tại X = 100
    float groundY = MapGetGroundY(map, 100.0f, -9999.0f);
    return (Vector2){100, groundY};
}

float MapGetCeilingY(cute_tiled_map_t* map, float x, float currentY) {
    if (!map) return -9999.0f;
    cute_tiled_layer_t* l = map->layers;
    while (l) {
        if (strcmp(l->name.ptr, "ground") == 0 && l->objects) {
            cute_tiled_object_t* obj = l->objects;
            
            float candidates[128];
            int count = 0;
            
            while (obj) {
                if (obj->vert_count > 0) {
                    for (int i = 0; i < obj->vert_count; i++) {
                        int next = (i + 1) % obj->vert_count;
                        float x1 = obj->x + obj->vertices[i * 2];
                        float y1 = obj->y + obj->vertices[i * 2 + 1];
                        float x2 = obj->x + obj->vertices[next * 2];
                        float y2 = obj->y + obj->vertices[next * 2 + 1];
                        if (x1 == x2) continue;
                        if ((x1 <= x && x2 >= x) || (x2 <= x && x1 >= x)) {
                            float t = (x - x1) / (x2 - x1);
                            float y_at_x = y1 + t * (y2 - y1);
                            if (count < 128) {
                                candidates[count++] = y_at_x;
                            }
                        }
                    }
                } else if (obj->width > 0) {
                    float x1 = obj->x;
                    float x2 = obj->x + obj->width;
                    if (x >= x1 && x <= x2) {
                        if (count < 128) {
                            candidates[count++] = obj->y;
                        }
                    }
                } else {
                    if (count < 128) {
                        candidates[count++] = obj->y;
                    }
                }
                obj = obj->next;
            }
            
            if (count == 0) return -9999.0f;
            
            // Lấy trần nhà thấp nhất (Y lớn nhất) nằm phía trên người chơi (candidates[i] < currentY - 16.0f)
            float bestCeilingY = -9999.0f;
            for (int i = 0; i < count; i++) {
                if (candidates[i] < currentY - 16.0f) {
                    if (bestCeilingY == -9999.0f || candidates[i] > bestCeilingY) {
                        bestCeilingY = candidates[i];
                    }
                }
            }
            return bestCeilingY;
        }
        l = l->next;
    }
    return -9999.0f;
}

bool MapCheckWallCollision(cute_tiled_map_t* map, Vector2 pos) {
    if (!map) return false;
    
    // Kiểm tra các điểm ở 2 rìa trái/phải của mèo (ở độ cao 12px và 28px tính từ chân)
    Vector2 points[4] = {
        { pos.x - 12.0f, pos.y - 12.0f },
        { pos.x - 12.0f, pos.y - 28.0f },
        { pos.x + 12.0f, pos.y - 12.0f },
        { pos.x + 12.0f, pos.y - 28.0f }
    };
    
    cute_tiled_layer_t* l = map->layers;
    while (l) {
        if (strcmp(l->name.ptr, "ground") == 0 && l->objects) {
            cute_tiled_object_t* obj = l->objects;
            while (obj) {
                for (int p = 0; p < 4; p++) {
                    Vector2 pt = points[p];
                    
                    if (obj->vert_count > 0) {
                        // Thuật toán Ray-Casting kiểm tra điểm nằm trong đa giác vật cản
                        int inside = 0;
                        for (int i = 0, j = obj->vert_count - 1; i < obj->vert_count; j = i++) {
                            float xi = obj->x + obj->vertices[i * 2];
                            float yi = obj->y + obj->vertices[i * 2 + 1];
                            float xj = obj->x + obj->vertices[j * 2];
                            float yj = obj->y + obj->vertices[j * 2 + 1];
                            
                            if (((yi > pt.y) != (yj > pt.y)) &&
                                (pt.x < (xj - xi) * (pt.y - yi) / (yj - yi) + xi)) {
                                inside = !inside;
                            }
                        }
                        if (inside) return true;
                    } 
                    else if (obj->width > 0 && obj->height > 0) {
                        // Kiểm tra điểm nằm trong hình chữ nhật vật cản
                        if (pt.x >= obj->x && pt.x <= obj->x + obj->width &&
                            pt.y >= obj->y && pt.y <= obj->y + obj->height) {
                            return true;
                        }
                    }
                }
                obj = obj->next;
            }
        }
        l = l->next;
    }
    return false;
}

bool MapCheckLayerCollision(cute_tiled_map_t* map, const char* layerName, Rectangle rect) {
    if (!map) return false;
    cute_tiled_layer_t* l = map->layers;
    while (l) {
        if (strcmp(l->name.ptr, layerName) == 0 && l->objects) {
            cute_tiled_object_t* obj = l->objects;
            while (obj) {
                if (obj->visible) {
                    if (obj->vert_count == 0 && obj->width > 0 && obj->height > 0) {
                        Rectangle objRect = { obj->x, obj->y, obj->width, obj->height };
                        if (CheckCollisionRecs(rect, objRect)) {
                            return true;
                        }
                    }
                }
                obj = obj->next;
            }
            return false;
        }
        l = l->next;
    }
    return false;
}
