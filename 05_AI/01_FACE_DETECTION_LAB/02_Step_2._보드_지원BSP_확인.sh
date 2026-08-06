# Source: 01_FACE_DETECTION_LAB.md
# Section: Step 2. 보드 지원(BSP) 확인

idf.py set-target esp32s3
idf.py menuconfig
# Camera Sensor 설정에서 SIOD/SIOC/XCLK/VSYNC/HREF/PCLK/D0-D7 핀 번호를 실제 배선에 맞게 수정
