#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <mosquitto.h>

// 브로커 설정 (라즈베리파이 내부 로컬 접속)
#define MQTT_HOST "127.0.0.1"
#define MQTT_PORT 1883
#define MQTT_KEEP_ALIVE 60

// 웹에서 요청한 타입만 추적하기 위한 플래그 배열
// AI, AO, AV, BI, BO, BV, MSV, SYS (총 8개)
bool active_types[8] = {false, false, false, false, false, false, false, false};
const char *types[] = {"AI", "AO", "AV", "BI", "BO", "BV", "MSV", "SYS"};
const int num_types = 8;

int get_type_index(const char* type) {
    for (int i = 0; i < num_types; i++) {
        if (strcmp(types[i], type) == 0) return i;
    }
    return -1;
}

// 브로커 접속 완료 시 호출되는 콜백 함수
void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    if (rc == 0) {
        printf("✅ 브로커에 성공적으로 연결되었습니다.\n");
        // 요청 및 제어 토픽 구독
        mosquitto_subscribe(mosq, NULL, "bacnet/request/#", 0);
        mosquitto_subscribe(mosq, NULL, "bacnet/command/#", 0);
        printf("📡 웹 요청(subscribe/unsubscribe) 및 제어(write) 토픽 수신 대기 중...\n");
    } else {
        printf("❌ 연결 실패, 반환 코드: %d\n", rc);
    }
}

// 메시지 수신 시 호출되는 콜백 함수
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    // 1. 데이터 구독 요청 처리
    if (strncmp(msg->topic, "bacnet/request/subscribe/", 25) == 0) {
        const char *type = msg->topic + 25;
        int idx = get_type_index(type);
        if (idx >= 0) {
            active_types[idx] = true;
            printf("▶️  웹 요청 수신: [%s] 데이터 전송 시작\n", type);
        }
    } 
    // 2. 데이터 구독 해제 요청 처리
    else if (strncmp(msg->topic, "bacnet/request/unsubscribe/", 27) == 0) {
        const char *type = msg->topic + 27;
        int idx = get_type_index(type);
        if (idx >= 0) {
            active_types[idx] = false;
            printf("⏸  웹 요청 수신: [%s] 데이터 전송 중지\n", type);
        }
    }
    // 3. 제어(Write) 명령 처리
    else if (strncmp(msg->topic, "bacnet/command/write/", 21) == 0) {
        // 토픽 형식: bacnet/command/write/<TYPE>/<ID>
        const char *path = msg->topic + 21;
        char type[16] = {0};
        char id[32] = {0};
        
        // type과 id 추출
        if (sscanf(path, "%15[^/]/%31s", type, id) == 2) {
            // 페이로드 파싱 (형식: "value,priority")
            if (msg->payloadlen > 0) {
                char payload_str[256] = {0};
                snprintf(payload_str, sizeof(payload_str), "%.*s", msg->payloadlen, (char*)msg->payload);
                
                char value[128] = {0};
                int priority = 0; // BACnet 우선순위 (1~16, 0이면 생략 등)
                
                if (sscanf(payload_str, "%[^,],%d", value, &priority) >= 1) {
                    printf("\n⚡ [제어 명령 수신] 대상: %s:%s | 변경 값(Present Value): %s | 우선순위(Priority): %d\n\n", 
                           type, id, value, priority);
                } else {
                    printf("⚠️ 제어 페이로드 파싱 실패: %s\n", payload_str);
                }
            }
        }
    }
}

// Publish 완료 시 호출되는 콜백 함수 (로그가 너무 많아지므로 주석 처리하거나 단순화)
void on_publish(struct mosquitto *mosq, void *obj, int mid) {
    // printf("📨 메시지(ID: %d) 전송 완료.\n", mid);
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
    mosquitto_message_callback_set(mosq, on_message);

    // 3. 브로커 연결
    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, MQTT_KEEP_ALIVE);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "연결할 수 없습니다: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    // 백그라운드 스레드 시작 (자동으로 네트워크 패킷 송수신/Ping 처리)
    mosquitto_loop_start(mosq);

    // 4. 무한 루프를 돌면서 웹에서 요청(active_types)한 타입의 데이터만 퍼블리시
    int cycle = 0;

    while (1) {
        for (int t = 0; t < num_types; t++) {
            // 웹에서 해당 타입의 화면을 보고 있을 때(플래그 true)만 데이터 전송
            if (active_types[t]) {
                char payload[32768];
                
                if (strcmp(types[t], "SYS") == 0) {
                    int cpu = 20 + (cycle % 20);
                    int mem = 60 + (cycle % 10);
                    snprintf(payload, sizeof(payload), 
                        "{\"bacnetInstance\":\"BACnet-04\",\"systemStatus\":\"Normal\",\"totalObjects\":1248,\"modules\":42,\"activeAlarms\":3,\"cpuUsage\":%d,\"memoryUsage\":%d}",
                        cpu, mem);
                } else {
                    strcpy(payload, "[");
                    
                    for (int i = 1; i <= 100; i++) {
                        char obj[256];
                        
                        if (strcmp(types[t], "AI") == 0 || strcmp(types[t], "AO") == 0 || strcmp(types[t], "AV") == 0) {
                            snprintf(obj, sizeof(obj), 
                                "{\"id\":\"%s:%d\",\"name\":\"Analog Node %d\",\"port\":\"MSTP:1\",\"presentValue\":%.1f,\"units\":\"°C\",\"status\":\"%s\",\"reliability\":\"No Fault\"}%s", 
                                types[t], i, i, 20.0 + (i * 0.1) + (cycle * 0.5), (i % 15 == 0) ? "Alarm" : "Normal", (i == 100) ? "" : ",");
                        } else if (strcmp(types[t], "BI") == 0 || strcmp(types[t], "BO") == 0 || strcmp(types[t], "BV") == 0) {
                            snprintf(obj, sizeof(obj), 
                                "{\"id\":\"%s:%d\",\"name\":\"Binary Node %d\",\"port\":\"MSTP:1\",\"presentValue\":\"%s\",\"status\":\"%s\",\"reliability\":\"No Fault\"}%s", 
                                types[t], i, i, ((i + cycle) % 2 == 0) ? "Active" : "Inactive", (i % 15 == 0) ? "FAULT" : "NORMAL", (i == 100) ? "" : ",");
                        } else if (strcmp(types[t], "MSV") == 0) {
                            snprintf(obj, sizeof(obj), 
                                "{\"id\":\"%s:%d\",\"name\":\"MultiState Node %d\",\"port\":\"MSTP:1\",\"presentValue\":%d,\"states\":%d,\"status\":\"%s\",\"reliability\":\"No Fault\"}%s", 
                                types[t], i, i, ((i + cycle) % 4) + 1, 4, (i % 15 == 0) ? "FAULT" : "NORMAL", (i == 100) ? "" : ",");
                        }
                        
                        strcat(payload, obj);
                    }
                    strcat(payload, "]");
                }

                char topic[64];
                snprintf(topic, sizeof(topic), "bacnet/objects/update/%s", types[t]);
                
                // 데이터 Publish (qos 0, retain false)
                rc = mosquitto_publish(mosq, NULL, topic, strlen(payload), payload, 0, false);
                if (rc != MOSQ_ERR_SUCCESS) {
                    fprintf(stderr, "퍼블리시 에러 (%s): %s\n", types[t], mosquitto_strerror(rc));
                }
            }
        }
        
        sleep(3); // 3초 대기
        cycle++;
    }

    // 5. 종료 전 리소스 정리 (현재 무한루프라 도달하지 않음)
    mosquitto_loop_stop(mosq, true);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
