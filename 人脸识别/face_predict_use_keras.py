#!/usr/bin/env python
#-*- coding:utf-8 -*-

import cv2
import sys
import gc
from face_train_use_keras import Model
import numpy as np
blockSize=64

#################################################################
#################光照补偿函数################################
def unevenLightCompensate(img, blockSize):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    average = np.mean(gray)

    rows_new = int(np.ceil(gray.shape[0] / blockSize))
    cols_new = int(np.ceil(gray.shape[1] / blockSize))

    blockImage = np.zeros((rows_new, cols_new), dtype=np.float32)
    for r in range(rows_new):
        for c in range(cols_new):
            rowmin = r * blockSize
            rowmax = (r + 1) * blockSize
            if (rowmax > gray.shape[0]):
                rowmax = gray.shape[0]
            colmin = c * blockSize
            colmax = (c + 1) * blockSize
            if (colmax > gray.shape[1]):
                colmax = gray.shape[1]

            imageROI = gray[rowmin:rowmax, colmin:colmax]
            temaver = np.mean(imageROI)
            blockImage[r, c] = temaver

    blockImage = blockImage - average
    blockImage2 = cv2.resize(blockImage, (gray.shape[1], gray.shape[0]), interpolation=cv2.INTER_CUBIC)
    gray2 = gray.astype(np.float32)
    dst = gray2 - blockImage2
    dst = dst.astype(np.uint8)
    dst = cv2.GaussianBlur(dst, (3, 3), 0)
    dst = cv2.cvtColor(dst, cv2.COLOR_GRAY2BGR)

    return dst
############################################################################################
############################################################################################

if __name__ == '__main__':


    # 加载模型
    model = Model()
    model.load_model(file_path='./model/test.face.model.h5')

    # 框住人脸的矩形边框颜色
    color = (0, 255, 0)

    # 捕获指定摄像头的实时视频流
    cap = cv2.VideoCapture(0)

    # 人脸识别分类器本地存储路径
    cascade_path = "C:\\Users\\Administrator\\Desktop\\Graduation design\\code\\csdn\\haarcascade_frontalface_alt2.xml"

    # 循环检测识别人脸
    while True:
        _, frame = cap.read()  # 读取一帧视频
        # frame=cv2.imread("dupei.jpg")#########################################################################################################测试使用

        # 图像灰化，降低计算复杂度
        frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # 使用人脸识别分类器，读入分类器
        cascade = cv2.CascadeClassifier(cascade_path)

        # 利用分类器识别出哪个区域为人脸
        faceRects = cascade.detectMultiScale(frame_gray, scaleFactor=1.2, minNeighbors=3, minSize=(32, 32))
        if len(faceRects) > 0:
            for faceRect in faceRects:
                x, y, w, h = faceRect
                ########################如果图像截取后，左边界大于右边界或者上边界大于下边界。则不执行#####################
                if x+20>=x+w-20 or y+30>=y+h-8:
                    pass
                ##########################################################################################################
                else:

                # 截取脸部图像提交给模型识别这是谁
                # image = frame[y + 40: y + h - 8, x + 30: x + w - 50]
                    image = frame[y + 30: y + h - 8, x + 20: x + w - 20]
                    image = cv2.medianBlur(image,5)  #################################################################################消除椒盐噪音
                    image = unevenLightCompensate(image, blockSize)################################################################进行光照补偿

                    result,faceID = model.face_predict(image)

                    # 如果是“我”
                    if faceID == 0 and result[0][0]>=0.90:
                    # if faceID == 0 :
                        cv2.rectangle(frame, (x - 10, y - 10), (x + w + 10, y + h + 10), color, thickness=2)

                        # 文字提示是谁
                        cv2.putText(frame, 'lijinlong',
                                    (x + 30, y + 30),  # 坐标
                                    cv2.FONT_HERSHEY_SIMPLEX,  # 字体
                                    1,  # 字号
                                    (255, 0, 255),  # 颜色
                                    2)  # 字的线宽
                    elif faceID == 1 and result[0][1]>=0.70:
                        cv2.rectangle(frame, (x - 10, y - 10), (x + w + 10, y + h + 10), color, thickness=2)

                        # 文字提示是谁
                        cv2.putText(frame, 'yangguang',
                                    (x + 30, y + 30),  # 坐标
                                    cv2.FONT_HERSHEY_SIMPLEX,  # 字体
                                    1,  # 字号
                                    (255, 0, 255),  # 颜色
                                    2)  # 字的线宽
                    elif faceID == 2 and result[0][2]>=0.90:
                        cv2.rectangle(frame, (x - 10, y - 10), (x + w + 10, y + h + 10), color, thickness=2)

                        # 文字提示是谁
                        cv2.putText(frame, 'leihexing',
                                    (x + 30, y + 30),  # 坐标
                                    cv2.FONT_HERSHEY_SIMPLEX,  # 字体
                                    1,  # 字号
                                    (255, 0, 255),  # 颜色
                                    2)  # 字的线宽
                    elif faceID == 3 and result[0][3] >= 0.90:
                        cv2.rectangle(frame, (x - 10, y - 10), (x + w + 10, y + h + 10), color, thickness=2)

                        # 文字提示是谁
                        cv2.putText(frame, 'LiangXiangDong',
                                    (x + 30, y + 30),  # 坐标
                                    cv2.FONT_HERSHEY_SIMPLEX,  # 字体
                                    1,  # 字号
                                    (255, 0, 255),  # 颜色
                                    2)  # 字的线宽
                    elif faceID == 4 and result[0][3] >= 0.90:
                        cv2.rectangle(frame, (x - 10, y - 10), (x + w + 10, y + h + 10), color, thickness=2)

                        # 文字提示是谁
                        cv2.putText(frame, 'YinYueLong',
                                    (x + 30, y + 30),  # 坐标
                                    cv2.FONT_HERSHEY_SIMPLEX,  # 字体
                                    1,  # 字号
                                    (255, 0, 255),  # 颜色
                                    2)  # 字的线宽
                    elif faceID == 5 and result[0][3] >= 0.90:
                        cv2.rectangle(frame, (x - 10, y - 10), (x + w + 10, y + h + 10), color, thickness=2)

                        # 文字提示是谁
                        cv2.putText(frame, 'WuYangWei',
                                    (x + 30, y + 30),  # 坐标
                                    cv2.FONT_HERSHEY_SIMPLEX,  # 字体
                                    1,  # 字号
                                    (255, 0, 255),  # 颜色
                                    2)  # 字的线宽
                    else:
                        pass

        cv2.imshow("face detection", frame)

        # 等待10毫秒看是否有按键输入
        k = cv2.waitKey(10)
        # 如果输入q则退出循环
        if k & 0xFF == ord('q'):
            break

    # 释放摄像头并销毁所有窗口
    cap.release()
    cv2.destroyAllWindows()