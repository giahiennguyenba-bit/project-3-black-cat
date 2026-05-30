#ifndef MAP_H
#define MAP_H

#include "cute_tiled.h"
#include "raylib.h"

// --- API Quản lý Bản đồ ---

// Nạp dữ liệu bản đồ từ file .tmj
cute_tiled_map_t* MapLoad(const char* filename);

// Giải phóng bản đồ và bộ nhớ đệm hình ảnh
void MapUnload(cute_tiled_map_t* map);

// Vẽ một layer cụ thể của bản đồ
void MapDrawLayer(cute_tiled_map_t* map, const char* layerName, float offsetX, Texture2D defaultTileset);

// Lấy tọa độ Y của mặt đất từ layer "ground" tại vị trí X cụ thể, căn cứ theo tọa độ Y hiện tại của nhân vật
float MapGetGroundY(cute_tiled_map_t* map, float x, float currentY);

// Lấy điểm Spawn từ layer "spawn" của bản đồ
Vector2 MapGetSpawnPoint(cute_tiled_map_t* map);

// Lấy tọa độ Y của trần nhà tại vị trí X cụ thể
float MapGetCeilingY(cute_tiled_map_t* map, float x, float currentY);

// Kiểm tra va chạm tường ngang tại vị trí cụ thể
bool MapCheckWallCollision(cute_tiled_map_t* map, Vector2 pos);

// Kiểm tra va chạm layer
bool MapCheckLayerCollision(cute_tiled_map_t* map, const char* layerName, Rectangle rect);

// Xử lý đường dẫn hình ảnh từ Tiled sang Raylib
void MapFixPath(char* path);

#endif // MAP_H
