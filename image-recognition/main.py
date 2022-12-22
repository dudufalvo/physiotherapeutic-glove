import cv2
import time
import HandTrackingModule as htm
import serial
import time

ser = serial.Serial("COM15", 9600)
ser.reset_input_buffer()

lastMode = mode = ""

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

        mode = ""
        
        for finger in fingers:
            mode += str(finger)
        
        currentTime = time.time()

        if(mode != lastMode) and (currentTime - previousTime > 1):
            command = ['I', mode, ';']
            

    cv2.putText(img, f'MODE: {mode}', (0, 30), cv2.FONT_HERSHEY_PLAIN, 2, (0, 255, 0), 3)

    cv2.imshow("Image", img)
    cv2.waitKey(1)
    