import cv2
import time
import HandTrackingModule as htm

lastMode = mode = 0

wCam, hCam = 640, 480

cap = cv2.VideoCapture(0)
cap.set(3, wCam)
cap.set(4, hCam)

previousTime = 0

detector = htm.handDetector(detectionCon=0.75)
tipIds = [4, 8, 12, 16, 20]

while True:
    success, img = cap.read()
    img = detector.findHands(img)
    landmarkList = detector.findPosition(img, draw=False)

    if len(landmarkList) != 0:
        fingers = []

        # Thumb
        if landmarkList[tipIds[0]][1] > landmarkList[tipIds[0] - 1][1]:
            fingers.append(1)
        else:
            fingers.append(0)

        # 4 Fingers
        for id in range(1, 5):
            if landmarkList[tipIds[id]][2] < landmarkList[tipIds[id]-2][2]:
                fingers.append(1)
            else:
                fingers.append(0)
        
        totalFingers = fingers.count(1)

        # all fingers down
        if fingers == [0, 0, 0, 0, 0]:
            mode = 1
            
        # all fingers up
        elif fingers == [1, 1, 1, 1, 1]:
            mode = 2

        elif totalFingers == 1:
            for i in range(5):
                if fingers[i] == 1:
                    mode = i + 3

        # 2 fingers up
        elif fingers == [1, 1, 0, 0, 0]:
            mode = 8
        elif fingers == [1, 0, 1, 0, 0]:
            mode = 9
        elif fingers == [1, 0, 0, 1, 0]:
            mode = 10
        elif fingers == [1, 0, 0, 0, 1]:
            mode = 11
        elif fingers == [0, 1, 1, 0, 0]:
            mode = 12
        elif fingers == [0, 1, 0, 1, 0]:
            mode = 13
        elif fingers == [0, 1, 0, 0, 1]:
            mode = 14
        elif fingers == [0, 0, 1, 1, 0]:
            mode = 15
        elif fingers == [0, 0, 1, 0, 1]:
            mode = 16
        elif fingers == [0, 0, 0, 1, 1]:
            mode = 17
        # 3 fingers up
        elif fingers == [1, 1, 1, 0, 0]:
            mode = 18
        elif fingers == [1, 1, 0, 1, 0]:
            mode = 19
        elif fingers == [1, 1, 0, 0, 1]:
            mode = 20
        elif fingers == [1, 0, 1, 1, 0]:
            mode = 21
        elif fingers == [1, 0, 1, 0, 1]:
            mode = 22
        elif fingers == [1, 0, 0, 1, 1]:
            mode = 23
        elif fingers == [0, 1, 1, 1, 0]:
            mode = 24
        elif fingers == [0, 1, 1, 0, 1]:
            mode = 25
        elif fingers == [0, 1, 0, 1, 1]:
            mode = 26
        elif fingers == [0, 0, 1, 1, 1]:
            mode = 27
        # 4 fingers up
        elif fingers == [1, 1, 1, 1, 0]:
            mode = 28
        elif fingers == [1, 1, 1, 0, 1]:
            mode = 29
        elif fingers == [1, 1, 0, 1, 1]:
            mode = 30
        elif fingers == [1, 0, 1, 1, 1]:
            mode = 31
        elif fingers == [0, 1, 1, 1, 1]:
            mode = 32

        
        if (mode != lastMode):
            print(mode)

        lastMode = mode

    currentTime = time.time()
    fps = 1 / (currentTime - previousTime)
    previousTime = currentTime

    cv2.putText(img, f'FPS: {int(fps)}', (0, 30), cv2.FONT_HERSHEY_PLAIN, 2, (0, 255, 0), 3)
    cv2.putText(img, f'MODE: {int(mode)}', (0, 60), cv2.FONT_HERSHEY_PLAIN, 2, (0, 255, 0), 3)

    cv2.imshow("Image", img)
    cv2.waitKey(1)
