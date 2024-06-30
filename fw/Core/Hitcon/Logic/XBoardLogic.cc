#include <cstring>
#include <Logic/XBoardLogic.h>

using namespace hitcon::service::sched;
using namespace hitcon::service::xboard;

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    uart_recv_cb(huart);
}*/

namespace hitcon {
namespace service {
namespace xboard {

constexpr size_t PKT_DATA_LEN_MAX = 256;
static uint8_t packet_data[PKT_DATA_LEN_MAX];

static callback_t packet_cb = nullptr;
static void * packet_cb_self = nullptr;
static PacketCallbackArg *packet_cb_arg;

struct Frame {
  uint64_t preamble; // 0xD555555555555555
	uint16_t id;
	uint8_t len;
	uint8_t type;
	uint16_t checksum;
};
constexpr size_t HEADER_SZ = sizeof(Frame);
//struct PostFrame {
//  uint32_t interpacket_gap_1;
//  uint32_t interpacket_gap_2;
//  uint32_t interpacket_gap_3;
//};

static int rx_tail = 0;
static uint8_t rx_buf_byte;
uint8_t tx_buf[] = "Hello World";
uint8_t rx_buf[RX_BUF_SZ] = {0};
UsartConnectState connect_state = Init;
static int rx_head = 0;

uint8_t alive_message[] = "alive_message";
void send_ping() {
  uint8_t pkt[sizeof(Frame)] = { 0 };
  *(Frame*)pkt = Frame {
    0xD555555555555555,
    0,
    sizeof(pkt),
    1,
    0
  };
  HAL_UART_Transmit(&huart2, (const uint8_t *)&pkt, sizeof(pkt), 100);
}

// don't move rx_head
// return false if no enough bytes to read
bool try_read_bytes(uint8_t *dst, size_t size) {
	unsigned int remain_size = (rx_tail > rx_head ? 0 : RX_BUF_SZ) + rx_tail - rx_head;
	if (remain_size < size) {
		return false;
	}
	if (rx_tail < rx_head) {
		unsigned int sz1 = RX_BUF_SZ - rx_head;
		memcpy(dst, rx_buf + rx_head, sz1);
		memcpy(dst + sz1, rx_buf, size - sz1);
		// rx_head = sz1;
	} else {
		memcpy(dst, rx_buf + rx_head, size);
		// rx_head += size;
	}
	return true;
}

inline void inc_rx_head(size_t size) {
	rx_head = (rx_head + size) % RX_BUF_SZ;
}

void parse_packet() {
	while (rx_head != rx_tail) {
		if (rx_buf[rx_head] != 0x55) {
			rx_head = (rx_head+1) % RX_BUF_SZ;
			continue;
		}
		unsigned int in_buf_size = (rx_tail > rx_head ? 0 : RX_BUF_SZ) + rx_tail - rx_head;
		if ((unsigned int)in_buf_size < sizeof(Frame)) {
			break;
		}

		Frame header;
		try_read_bytes((uint8_t *)&header, HEADER_SZ);
		if (header.preamble != 0xD555555555555555) {
			inc_rx_head(1);
			continue;
		}
		inc_rx_head(HEADER_SZ);
		if (!try_read_bytes((uint8_t *)&packet_data, header.len)) {
			// no enough bytes to read, wait more bytes in
			return;
		}
		inc_rx_head(header.len);
		if (packet_cb != nullptr) {
			memcpy(packet_cb_arg->data, &packet_data, header.len);
			packet_cb_arg->len = header.len;
			packet_cb(packet_cb_self, packet_cb_arg);
		}
	}
}

void uart_init() { uart_recv_cb(&huart2); }

void uart_recv_cb(UART_HandleTypeDef *huart) {
    int next_tail = (rx_tail+1) % RX_BUF_SZ;
    if (next_tail != rx_head) {
    	// only write when this hold, so head will not be overwritten
    	rx_buf[rx_tail] = rx_buf_byte;
    	rx_tail = next_tail;
    }
    HAL_UART_Receive_IT(&huart2, &rx_buf_byte, 1);
}


XBoardLogic::XBoardLogic() {
  uart_init();
  auto task = PeriodicTask(1, (task_callback_t)&XBoardLogic::Routine, this, 100);
  scheduler.Queue(&task, nullptr);
  scheduler.EnablePeriodic(&task);
  scheduler.Run();
}

void XBoardLogic::QueuePacketForTx(uint8_t* packet, size_t packet_len) {
  // TODO
}

void XBoardLogic::SetOnConnectCallback(callback_t callback, void* callback_arg1) {
  // TODO
}

void XBoardLogic::SetOnDisconnectCallback(callback_t callback, void* callback_arg1) {
  // TODO
}

void XBoardLogic::SetOnPacketCallback(callback_t callback, void* self, PacketCallbackArg *callback_arg) {
  // TODO
	packet_cb = callback;
	packet_cb_self = self;
	packet_cb_arg = callback_arg;
}

void XBoardLogic::Routine(void*) {
  // TODO
  send_ping();
  parse_packet();
}

}
}
}  // namespace hitcon
