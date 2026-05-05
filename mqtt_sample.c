#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>

// 브로커 설정 (라즈베리파이 내부 로컬 접속)
#define MQTT_HOST "127.0.0.1"
#define MQTT_PORT 1883
#define MQTT_KEEP_ALIVE 60

// 브로커 접속 완료 시 호출되는 콜백 함수
void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    if (rc == 0) {
        printf("✅ 브로커에 성공적으로 연결되었습니다.\n");
    } else {
        printf("❌ 연결 실패, 반환 코드: %d\n", rc);
    }
}

// Publish 완료 시 호출되는 콜백 함수
void on_publish(struct mosquitto *mosq, void *obj, int mid) {
    printf("📨 메시지(ID: %d) 전송 완료.\n", mid);
}

int main() {
    struct mosquitto *mosq = NULL;
    int rc;

    // 1. Mosquitto 라이브러리 초기화
    mosquitto_lib_init();

    // 2. 새 클라이언트 인스턴스 생성 (true = clean session)
    mosq = mosquitto_new("bacnet_c_publisher", true, NULL);
    if (!mosq) {
        fprintf(stderr, "오류: 메모리 할당 실패.\n");
        return 1;
    }

    // 콜백 함수 등록
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_publish_callback_set(mosq, on_publish);

    // 3. 브로커 연결
    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, MQTT_KEEP_ALIVE);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "연결할 수 없습니다: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    // 백그라운드 스레드 시작 (자동으로 네트워크 패킷 송수신/Ping 처리)
    mosquitto_loop_start(mosq);

    // 4. 테스트를 위해 5번 주기적으로 데이터를 Publish 합니다.
    int count = 0;
    const char *types[] = {"AI", "AO", "AV", "BI", "BO", "BV", "MSV"};
    int num_types = 7;

    while (count < 5) {
        for (int t = 0; t < num_types; t++) {
            // 각 타입별로 100개의 객체를 모방한 JSON 배열 생성
            char payload[32768];
            strcpy(payload, "[");
            
            for (int i = 1; i <= 100; i++) {
                char obj[256];
                
                if (strcmp(types[t], "AI") == 0 || strcmp(types[t], "AO") == 0 || strcmp(types[t], "AV") == 0) {
                    snprintf(obj, sizeof(obj), 
                        "{\"id\":\"%s:%d\",\"name\":\"Analog Node %d\",\"port\":\"MSTP:1\",\"presentValue\":%.1f,\"units\":\"°C\",\"status\":\"%s\",\"reliability\":\"No Fault\"}%s", 
                        types[t], i, i, 20.0 + (i * 0.1) + (count * 0.5), (i % 15 == 0) ? "Alarm" : "Normal", (i == 100) ? "" : ",");
                } else if (strcmp(types[t], "BI") == 0 || strcmp(types[t], "BO") == 0 || strcmp(types[t], "BV") == 0) {
                    snprintf(obj, sizeof(obj), 
                        "{\"id\":\"%s:%d\",\"name\":\"Binary Node %d\",\"port\":\"MSTP:1\",\"presentValue\":\"%s\",\"status\":\"%s\",\"reliability\":\"No Fault\"}%s", 
                        types[t], i, i, (i % 2 == 0) ? "Active" : "Inactive", (i % 15 == 0) ? "FAULT" : "NORMAL", (i == 100) ? "" : ",");
                } else if (strcmp(types[t], "MSV") == 0) {
                    snprintf(obj, sizeof(obj), 
                        "{\"id\":\"%s:%d\",\"name\":\"MultiState Node %d\",\"port\":\"MSTP:1\",\"presentValue\":%d,\"states\":%d,\"status\":\"%s\",\"reliability\":\"No Fault\"}%s", 
                        types[t], i, i, (i % 4) + 1, 4, (i % 15 == 0) ? "FAULT" : "NORMAL", (i == 100) ? "" : ",");
                }
                
                strcat(payload, obj);
            }
            strcat(payload, "]");

            char topic[64];
            snprintf(topic, sizeof(topic), "bacnet/objects/update/%s", types[t]);
            
            printf("[%d/5] 토픽 '%s' 으로 100개의 데이터를 퍼블리시 중...\n", count+1, topic);
            
            // 데이터 Publish (qos 0, retain false)
            rc = mosquitto_publish(mosq, NULL, topic, strlen(payload), payload, 0, false);
            if (rc != MOSQ_ERR_SUCCESS) {
                fprintf(stderr, "퍼블리시 에러 (%s): %s\n", types[t], mosquitto_strerror(rc));
            }
        }
        
        printf("----------------------------------------\n");
        sleep(3); // 3초 대기
        count++;
    }

    // 5. 종료 전 리소스 정리
    printf("\n테스트 종료. 연결을 해제합니다.\n");
    mosquitto_loop_stop(mosq, true);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
