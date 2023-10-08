# 重み付きyの値を計算する関数
def weighted_y(y, height, weight):
    return abs(y - height / 2) * weight

# 長方形の範囲を定義する
rect_left = width * 0.4 # 画像の中心から左に10%
rect_right = width * 0.6 # 画像の中心から右に10%
rect_top = height * 0.4 # 画像の中心から上に10%
rect_bottom = height * 0.6 # 画像の中心から下に10%

# 検出された物体ごとに優先度を計算する
for i in range(len(boxes.xyxy)):
    x1, y1, x2, y2 = boxes.xyxy[i]
    x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
    center_x = (x1 + x2) / 2
    center_y = (y1 + y2) / 2
    priority = abs(center_x - width / 2) + weighted_y(center_y, height, 0.5) # xと重み付きyの和を優先度とする
    if rect_left < center_x < rect_right and rect_top < center_y < rect_bottom: # センターの内側にある場合
        priority *= 0.5 # 優先度を半分にする
    boxes.priority[i] = priority # 優先度をboxesオブジェクトに追加する

# 優先度が最小（最も優先される）物体を選択する
min_index = boxes.priority.index(min(boxes.priority)) # 優先度が最小のインデックスを取得する
x1, y1, x2, y2 = boxes.xyxy[min_index] # 選択された物体の座標を取得する
state = 1 # 物体が検出されたことを示すフラグ
move_value = round(center_x / width * 100 - 50) # 物体の位置に応じて動かすべき方向と量を計算する
x = move_value # xに代入する
y = round((y1+y2)/2/height*100) # 物体の距離に応じて動かすべき方向と量を計算する
kind = int(boxes.cls[min_index]) # 物体の種類を取得する
conf = int(f'{boxes.conf[min_index].item()*100:.0f}') # 物体の信頼度を取得する
