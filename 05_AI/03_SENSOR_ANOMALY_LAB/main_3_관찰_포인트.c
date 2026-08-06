// Source: 03_SENSOR_ANOMALY_LAB.md
// Section: 관찰 포인트

  // 20ms마다 샘플링하는 대신, 예: 400ms 윈도우 동안의 이벤트 카운트를 특징값으로 사용
  volatile int event_count = 0;   // ISR에서 증가
  // ... 윈도우 종료 시 event_count를 window[i] 자리에 넣고 0으로 리셋
  
