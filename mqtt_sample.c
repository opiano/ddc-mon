#include <mosquitto.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// 브로커 설정 (라즈베리파이 내부 로컬 접속)
#define MQTT_HOST "127.0.0.1"
#define MQTT_PORT 1883
#define MQTT_KEEP_ALIVE 60

// 웹에서 요청한 타입만 추적하기 위한 플래그 배열
// AI, AO, AV, BI, BO, BV, MSV, DEV, CAL, SCH, TLOG, FBD, DEVLST (총 13개)
bool active_types[13] = {false, false, false, false, false, false, false, false, false, false, false, false, false};
const char *types[] = {"AI", "AO", "AV", "BI", "BO", "BV", "MSV", "DEV", "CAL", "SCH", "TLOG", "FBD", "DEVLST"};
const int num_types = 13;

int get_type_index(const char *type) {
  for (int i = 0; i < num_types; i++) {
    if (strcmp(types[i], type) == 0)
      return i;
  }
  return -1;
}



int main() {
  struct mosquitto *mosq = NULL;
  int rc;

  time_t boot_time = time(NULL);
  char boot_time_str[64];
  strftime(boot_time_str, sizeof(boot_time_str), "%Y-%m-%d %H:%M:%S",
           localtime(&boot_time));

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

        if (strcmp(types[t], "DEV") == 0) {
          int cpu = get_cpu_usage();
          int mem = get_memory_usage();
          double cpu_temp = get_cpu_temperature();
          time_t now = time(NULL);
          char sys_time_str[64];
          strftime(sys_time_str, sizeof(sys_time_str), "%Y-%m-%d %H:%M:%S",
                   localtime(&now));
          long uptime = (long)difftime(now, boot_time);

          const char *ev1_ts = "2026-05-09 14:02:45";
          const char *ev1_state = "Warning";
          const char *ev1_name = "High Temp AHU-01";
          const char *ev1_id = "Node-04";

          const char *ev2_ts = "2026-05-09 13:58:12";
          const char *ev2_state = "Info";
          const char *ev2_name = "System Backup Completed";
          const char *ev2_id = "Master-Ctrl";

          const char *ev3_ts = "2026-05-09 13:45:33";
          const char *ev3_state = "Critical";
          const char *ev3_name = "Loss of Communication - Floor 4";
          const char *ev3_id = "Router-F4";

          const char *ev4_ts = "2026-05-09 13:30:01";
          const char *ev4_state = "Info";
          const char *ev4_name = "Scheduled Lighting Override: Active";
          const char *ev4_id = "Node-04";

          const char *ev5_ts = "2026-05-09 13:15:00";
          const char *ev5_state = "Normal";
          const char *ev5_name = "System Startup";
          const char *ev5_id = "Master-Ctrl";

          char events_json[2048];
          snprintf(events_json, sizeof(events_json),
                   "["
                   "{\"timestamp\":\"%s\",\"eventState\":\"%s\",\"objectName\":"
                   "\"%s\",\"objectId\":\"%s\"},"
                   "{\"timestamp\":\"%s\",\"eventState\":\"%s\",\"objectName\":"
                   "\"%s\",\"objectId\":\"%s\"},"
                   "{\"timestamp\":\"%s\",\"eventState\":\"%s\",\"objectName\":"
                   "\"%s\",\"objectId\":\"%s\"},"
                   "{\"timestamp\":\"%s\",\"eventState\":\"%s\",\"objectName\":"
                   "\"%s\",\"objectId\":\"%s\"},"
                   "{\"timestamp\":\"%s\",\"eventState\":\"%s\",\"objectName\":"
                   "\"%s\",\"objectId\":\"%s\"}"
                   "]",
                   ev1_ts, ev1_state, ev1_name, ev1_id, ev2_ts, ev2_state,
                   ev2_name, ev2_id, ev3_ts, ev3_state, ev3_name, ev3_id,
                   ev4_ts, ev4_state, ev4_name, ev4_id, ev5_ts, ev5_state,
                   ev5_name, ev5_id);

          const char *bacnet_instance = "Device-100";
          const char *system_status = "Normal";
          int total_objects = 1248;
          int modules = 42;
          int active_alarms = 3;
          const char *hostname = "DDC";

          snprintf(payload, sizeof(payload),
                   "{\"bacnetInstance\":\"%s\",\"systemStatus\":\"%s\","
                   "\"totalObjects\":%d,\"modules\":%d,\"activeAlarms\":%d,"
                   "\"cpuUsage\":%d,\"memoryUsage\":%d,\"cpuTemp\":%.1f,\"hostname\":\"%s\","
                   "\"systemTime\":\"%s\",\"uptime\":%ld,\"bootTime\":\"%s\","
                   "\"events\":%s}",
                   bacnet_instance, system_status, total_objects, modules,
                   active_alarms, cpu, mem, cpu_temp, hostname, sys_time_str, uptime,
                   boot_time_str, events_json);
        } else if (strcmp(types[t], "DEVLST") == 0) {
          strcpy(payload, "[");
          for (int i = 1; i <= 8; i++) {
            char obj[512];
            int dev_instance = 100 + i * 10;
            int max_apdu = 1476;
            int max_apdu_accepted = 1476;
            int seg_supp = 3;
            int sa = 0x01 + i;
            int max_seg = 64;
            int vendor_id = 15;
            char dev_addr[64];
            snprintf(dev_addr, sizeof(dev_addr), "%d / 192.168.1.%d:47808", i, 10 + i);

            snprintf(obj, sizeof(obj),
                     "{\"deviceInstance\":%d,\"maxApdu\":%d,\"maxApduAccepted\":%d,"
                     "\"segSupp\":%d,\"sa\":%d,\"maxSeg\":%d,\"vendorId\":%d,"
                     "\"devAddr\":\"%s\"}%s",
                     dev_instance, max_apdu, max_apdu_accepted, seg_supp, sa,
                     max_seg, vendor_id, dev_addr,
                     (i == 8) ? "" : ",");
            strcat(payload, obj);
          }
          strcat(payload, "]");
        } else {
          strcpy(payload, "[");

          for (int i = 1; i <= 100; i++) {
            char obj[512];

            if (strcmp(types[t], "AI") == 0 || strcmp(types[t], "AO") == 0 ||
                strcmp(types[t], "AV") == 0) {
              int id = i;
              char name[64];
              snprintf(name, sizeof(name), "Analog Node %d", i);
              const char *port = "MSTP:1";
              float pv = 20.0 + (i * 0.1) + (cycle * 0.5);
              const char *units = "°C";
              const char *sts = (i % 15 == 0) ? "Alarm" : "Normal";
              const char *rel = "No Fault";
              const char *oos = (i % 20 == 0) ? "true" : "false";
              int pri = (i % 5 == 0) ? 8 : 16;
              int ed = (i % 10 == 0) ? 0 : 1;
              float hl = 80.0;
              float ll = 10.0;

              snprintf(obj, sizeof(obj),
                       "{\"id\":\"%s:%d\",\"name\":\"%s\",\"port\":\"%s\","
                       "\"pv\":%.1f,\"units\":\"%s\",\"sts\":\"%s\",\"rel\":\"%"
                       "s\",\"oos\":%s,\"pri\":%d,\"ed\":%d,"
                       "\"hl\":%.1f,\"ll\":%.1f}%s",
                       types[t], id, name, port, pv, units, sts, rel, oos, pri,
                       ed, hl, ll,
                       (i == 100) ? "" : ",");
            } else if (strcmp(types[t], "BI") == 0 ||
                       strcmp(types[t], "BO") == 0 ||
                       strcmp(types[t], "BV") == 0) {
              int id = i;
              char name[64];
              snprintf(name, sizeof(name), "Binary Node %d", i);
              const char *port = "MSTP:1";
              const char *pv = ((i + cycle) % 2 == 0) ? "Active" : "Inactive";
              const char *sts = (i % 15 == 0) ? "FAULT" : "NORMAL";
              const char *rel = "No Fault";
              const char *oos = (i % 20 == 0) ? "true" : "false";
              int pri = (i % 5 == 0) ? 8 : 16;
              int ed = (i % 10 == 0) ? 0 : 1;
              int av = 1;

              snprintf(
                  obj, sizeof(obj),
                  "{\"id\":\"%s:%d\",\"name\":\"%s\",\"port\":\"%s\",\"pv\":\"%"
                  "s\",\"sts\":\"%s\",\"rel\":\"%s\",\"oos\":%s,\"pri\":%d,"
                  "\"ed\":%d,\"av\":%d}%s",
                  types[t], id, name, port, pv, sts, rel, oos, pri,
                  ed, av,
                  (i == 100) ? "" : ",");
            } else if (strcmp(types[t], "MSV") == 0) {
              int id = i;
              char name[64];
              snprintf(name, sizeof(name), "MultiState Node %d", i);
              const char *port = "MSTP:1";
              int pv = ((i + cycle) % 4) + 1;
              int states = 4;
              const char *sts = (i % 15 == 0) ? "FAULT" : "NORMAL";
              const char *rel = "No Fault";
              const char *oos = (i % 20 == 0) ? "true" : "false";
              int pri = (i % 5 == 0) ? 8 : 16;
              int ed = (i % 10 == 0) ? 0 : 1;
              int av = 3;

              snprintf(obj, sizeof(obj),
                       "{\"id\":\"%s:%d\",\"name\":\"%s\",\"port\":\"%s\","
                       "\"pv\":%d,\"states\":%d,\"sts\":\"%s\",\"rel\":\"%s\","
                       "\"oos\":%s,\"pri\":%d,\"ed\":%d,"
                       "\"av\":%d}%s",
                       types[t], id, name, port, pv, states, sts, rel, oos, pri,
                       ed, av,
                       (i == 100) ? "" : ",");
            } else if (strcmp(types[t], "CAL") == 0) {
              int id = i;
              char name[64];
              snprintf(name, sizeof(name), "Calendar Node %d", i);
              int totalEntries = (i * 3) % 20;
              const char *pv = (i % 2 == 0) ? "True" : "False";

              snprintf(obj, sizeof(obj),
                       "{\"id\":\"%s:%d\",\"name\":\"%s\",\"totalEntries\":%d,\"pv\":\"%s\"}%s",
                       types[t], id, name, totalEntries, pv, (i == 100) ? "" : ",");
            } else if (strcmp(types[t], "SCH") == 0) {
              int id = i;
              char name[64];
              snprintf(name, sizeof(name), "Schedule Node %d", i);
              const char *pv = (i % 3 == 0) ? "Active" : "Inactive";
              const char *effectivePeriod = "2024-01-01 - 2024-12-31";
              const char *scheduleDefault = "Inactive";
              char objRef[32];
              snprintf(objRef, sizeof(objRef), "BO:%d", i);

              snprintf(obj, sizeof(obj),
                       "{\"id\":\"%s:%d\",\"name\":\"%s\",\"pv\":\"%s\",\"effPeriod\":\"%s\",\"schDef\":\"%s\",\"objRef\":\"%s\"}%s",
                       types[t], id, name, pv, effectivePeriod, scheduleDefault, objRef, (i == 100) ? "" : ",");
            } else if (strcmp(types[t], "TLOG") == 0) {
              int id = i;
              char name[64];
              snprintf(name, sizeof(name), "TrendLog Node %d", i);
              const char *enable = (i % 4 == 0) ? "False" : "True";
              int interval = 15;
              char logRef[32];
              snprintf(logRef, sizeof(logRef), "AI:%d", i);
              int recordCount = 100 + (cycle * 2) % 1000;
              int totalCount = 5000 + cycle * 2;

              snprintf(obj, sizeof(obj),
                       "{\"id\":\"%s:%d\",\"name\":\"%s\",\"enable\":\"%s\",\"interval\":%d,\"logRef\":\"%s\",\"rc\":%d,\"tc\":%d}%s",
                       types[t], id, name, enable, interval, logRef, recordCount, totalCount, (i == 100) ? "" : ",");
            } else if (strcmp(types[t], "FBD") == 0) {
              int id = i;
              char name[64];
              snprintf(name, sizeof(name), "FBD Node %d", i);
              const char *pv = ((i + cycle) % 2 == 0) ? "true" : "false";
              int period = 1000 + i * 10;
              int fb = 10 + i % 5;
              int var = 20 + i % 10;
              int link = i * 4 + 100;

              snprintf(obj, sizeof(obj),
                       "{\"id\":\"%s:%d\",\"name\":\"%s\",\"pv\":%s,\"period\":%d,\"fb\":%d,\"var\":%d,\"link\":%d}%s",
                       types[t], id, name, pv, period, fb, var, link, (i == 100) ? "" : ",");
            }

            strcat(payload, obj);
          }
          strcat(payload, "]");
        }

        char topic[64];
        snprintf(topic, sizeof(topic), "bacnet/objects/update/%s", types[t]);

        // 데이터 Publish (qos 0, retain false)
        rc = mosquitto_publish(mosq, NULL, topic, strlen(payload), payload, 0,
                               false);
        if (rc != MOSQ_ERR_SUCCESS) {
          fprintf(stderr, "퍼블리시 에러 (%s): %s\n", types[t],
                  mosquitto_strerror(rc));
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

// 브로커 접속 완료 시 호출되는 콜백 함수
void on_connect(struct mosquitto *mosq, void *obj, int rc) {
  if (rc == 0) {
    printf("✅ 브로커에 성공적으로 연결되었습니다.\n");
    // 요청 및 제어 토픽 구독
    mosquitto_subscribe(mosq, NULL, "bacnet/request/#", 0);
    mosquitto_subscribe(mosq, NULL, "bacnet/command/#", 0);
    printf("📡 웹 요청(subscribe/unsubscribe) 및 제어(write) 토픽 수신 대기 "
           "중...\n");
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
        snprintf(payload_str, sizeof(payload_str), "%.*s", msg->payloadlen,
                 (char *)msg->payload);

        char value[128] = {0};
        int priority = 0; // BACnet 우선순위 (1~16, 0이면 생략 등)

        if (sscanf(payload_str, "%[^,],%d", value, &priority) >= 1) {
          printf("\n⚡ [제어 명령 수신] 대상: %s:%s | 변경 값(Present Value): "
                 "%s | 우선순위(Priority): %d\n\n",
                 type, id, value, priority);
        } else {
          printf("⚠️ 제어 페이로드 파싱 실패: %s\n", payload_str);
        }
      }
    }
  }
  // 4. 알람 설정(Alarm) 명령 처리
  else if (strncmp(msg->topic, "bacnet/command/alarm/", 21) == 0) {
    // 토픽 형식: bacnet/command/alarm/<TYPE>/<ID>
    const char *path = msg->topic + 21;
    char type[16] = {0};
    char id[32] = {0};

    if (sscanf(path, "%15[^/]/%31s", type, id) == 2) {
      if (msg->payloadlen > 0) {
        char payload_str[256] = {0};
        snprintf(payload_str, sizeof(payload_str), "%.*s", msg->payloadlen,
                 (char *)msg->payload);

        int ed = 0;
        float hl = 0.0f;
        float ll = 0.0f;
        int av = 0;

        printf("\n[Alarm Config Command Received] Target: %s:%s | Payload: %s\n",
               type, id, payload_str);

        if (strcmp(type, "AI") == 0 || strcmp(type, "AO") == 0 || strcmp(type, "AV") == 0) {
          if (sscanf(payload_str, "%d,%f,%f", &ed, &hl, &ll) == 3) {
            printf("   Parse Success: Event Detection Enable (ed) = %d, High Limit (hl) = %.2f, Low Limit (ll) = %.2f\n\n", ed, hl, ll);
          } else {
            printf("   Parse Failure: Analog alarm payload format is invalid.\n\n");
          }
        } else if (strcmp(type, "BI") == 0 || strcmp(type, "BV") == 0) {
          if (sscanf(payload_str, "%d,%d", &ed, &av) == 2) {
            printf("   Parse Success: Event Detection Enable (ed) = %d, Alarm Value (av) = %d (%s)\n\n", ed, av, av == 1 ? "ACTIVE" : "INACTIVE");
          } else {
            printf("   Parse Failure: Binary alarm payload format is invalid.\n\n");
          }
        } else if (strcmp(type, "BO") == 0) {
          if (sscanf(payload_str, "%d", &ed) == 1) {
            printf("   Parse Success: Event Detection Enable (ed) = %d\n\n", ed);
          } else {
            printf("   Parse Failure: BO alarm payload format is invalid.\n\n");
          }
        } else if (strcmp(type, "MSV") == 0) {
          if (sscanf(payload_str, "%d,%d", &ed, &av) == 2) {
            printf("   Parse Success: Event Detection Enable (ed) = %d, Alarm Value (av State Index) = %d\n\n", ed, av);
          } else {
            printf("   Parse Failure: MSV alarm payload format is invalid.\n\n");
          }
        } else {
          printf("   Parse Failure: Unknown object type.\n\n");
        }
      }
    }
  }
  // 5. 기기 인스턴스 설정 명령 처리
  else if (strncmp(msg->topic, "bacnet/command/device/instance", 30) == 0) {
    if (msg->payloadlen > 0) {
      char payload_str[256] = {0};
      snprintf(payload_str, sizeof(payload_str), "%.*s", msg->payloadlen,
               (char *)msg->payload);
      int instance_val = atoi(payload_str);
      printf("\n⚙️ [디바이스 설정 명령 수신] 새 Device ID/Instance: %d (Integer)\n\n", instance_val);
    }
  }
  // 6. 시스템 시각 설정 명령 처리
  else if (strncmp(msg->topic, "bacnet/command/system/time", 26) == 0) {
    if (msg->payloadlen > 0) {
      char payload_str[256] = {0};
      snprintf(payload_str, sizeof(payload_str), "%.*s", msg->payloadlen,
               (char *)msg->payload);
      
      int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
      if (sscanf(payload_str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) == 6) {
        printf("\n⏰ [시스템 시각 설정 명령 수신] parsed: YYYY=%d, MM=%d, DD=%d, hh=%d, mm=%d, ss=%d (Integers)\n\n",
               year, month, day, hour, minute, second);
      } else {
        printf("\n⏰ [시스템 시각 설정 명령 수신] 새 System Time: %s (파싱 실패)\n\n", payload_str);
      }
    }
  }
  // 7. 수동 디바이스 추가 명령 처리
  else if (strncmp(msg->topic, "bacnet/command/device/add", 25) == 0) {
    if (msg->payloadlen > 0) {
      char payload_str[512] = {0};
      snprintf(payload_str, sizeof(payload_str), "%.*s", msg->payloadlen,
               (char *)msg->payload);
      
      int dev_inst = 0, max_apdu = 0, seg_supp = 0, vendor = 0;
      char dev_addr[128] = {0};
      
      if (sscanf(payload_str, "%d,%d,%d,%d,%127[^,\n]", 
                 &dev_inst, &max_apdu, &seg_supp, &vendor, dev_addr) == 5) {
        
        int net_num = 0;
        char mac_addr[128] = {0};
        
        if (sscanf(dev_addr, "%d / %127s", &net_num, mac_addr) == 2) {
          printf("\n⚡ [수동 디바이스 추가 명령 수신]\n"
                 "   Device Instance: %d (Integer)\n"
                 "   Max APDU       : %d (Integer)\n"
                 "   Seg Supp       : %d (Integer)\n"
                 "   Vendor ID      : %d (Integer)\n"
                 "   Network Number : %d (Integer)\n"
                 "   MAC Address    : %s (String)\n\n",
                 dev_inst, max_apdu, seg_supp, vendor, net_num, mac_addr);
        } else {
          printf("\n⚡ [수동 디바이스 추가 명령 수신]\n"
                 "   Device Instance: %d (Integer)\n"
                 "   Max APDU       : %d (Integer)\n"
                 "   Seg Supp       : %d (Integer)\n"
                 "   Vendor ID      : %d (Integer)\n"
                 "   Device Address : %s (String) (분리 실패)\n\n",
                 dev_inst, max_apdu, seg_supp, vendor, dev_addr);
        }
      } else {
        printf("\n⚡ [수동 디바이스 추가 명령 수신] payload: %s (파싱 실패)\n\n", payload_str);
      }
    }
  }
}



// Publish 완료 시 호출되는 콜백 함수 (로그가 너무 많아지므로 주석 처리하거나
// 단순화)
void on_publish(struct mosquitto *mosq, void *obj, int mid) {
  // printf("📨 메시지(ID: %d) 전송 완료.\n", mid);
}