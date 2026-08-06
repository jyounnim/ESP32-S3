// Source: 12_TASK_NOTIFICATION_LAB.md
// Section: 관찰 포인트

  // 신호를 보내는 쪽
  xTaskNotifyGive(consumerHandle);

  // 받는 쪽
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  
