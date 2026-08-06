// Source: 17_CRITICAL_SECTION_LAB.md
// Section: 핵심 개념

portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

portENTER_CRITICAL(&spinlock);
// 아주 짧은 보호 구간
portEXIT_CRITICAL(&spinlock);
