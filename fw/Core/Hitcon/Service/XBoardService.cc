#include <cstring>
#include <Service/XBoardService.h>

using namespace hitcon::service::sched;
using namespace hitcon::service::xboard;

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    uart_recv_cb(huart);
}
*/

namespace hitcon {
namespace service {
namespace xboard {

XBoardService g_xboard_service;

XBoardService::XBoardService() :
  on_rx_callback_(nullptr), on_rx_callback_arg1_(nullptr),
  rx_task_(480, (callback_t) &XBoardService::OnRxWrapper, this) {
}


void XBoardService::Init() {
  auto task = PeriodicTask(500, (task_callback_t)&XBoardService::Routine, this, 10);
  scheduler.Queue(&task, nullptr);
  scheduler.EnablePeriodic(&task);
  scheduler.Run();

  TriggerRx();
}

void XBoardService::TriggerRx() {
  HAL_UART_Receive_IT(&huart_, &rx_byte_, 1);
}

void XBoardService::SetOnByteRx(callback_t callback, void* callback_arg1) {
  on_rx_callback_ = callback;
  on_rx_callback_arg1_ = callback_arg1;
  rx_task_ = Task(480, callback, callback_arg1);
}

void XBoardService::NotifyRxFinish() {
  if (rx_task_busy_) {
    // Overflow, we dropped a byte.
  } else {
    if (on_rx_callback_) {
      rx_task_busy_ = true;
      scheduler.Queue(&rx_task_, reinterpret_cast<void*>(static_cast<size_t>(rx_byte_)));
    }
  }
  TriggerRx();
}

void XBoardService::OnRxWrapper(void* arg2) {
  rx_task_busy_ = false;
  if (on_rx_callback_) {
    on_rx_callback_(on_rx_callback_arg1_, arg2);
  }
}

void XBoardService::QueueDataForTx(uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (tx_buffer_tail_+1 == tx_buffer_head_) {
      // Overflow, we're dropping data.
      break;
    }
    tx_buffer_[tx_buffer_tail_] = data[i];
    tx_buffer_tail_ = (tx_buffer_tail_+1)%kTxBufferSize;
  }
}

void XBoardService::Routine(void *) {
  if (tx_buffer_head_ != tx_buffer_tail_ && !tx_busy_) {
    tx_busy_ = true;
    HAL_UART_Transmit_IT(&huart_, &tx_buffer_[tx_buffer_head_], 1);
    tx_buffer_head_ = (tx_buffer_head_+1)%kTxBufferSize;
  }
}

}  // namespace xboard
}  // namespace service
}  // namespace hitcon

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  g_xboard_service.NotifyTxFinish();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  g_xboard_service.NotifyRxFinish();
}

