#ifndef HITCON_SERVICE_XBOARD_SERVICE_H_
#define HITCON_SERVICE_XBOARD_SERVICE_H_

#include <Util/callback.h>
#include <stddef.h>
#include <stdint.h>

#include "Service/Sched/Scheduler.h"
#include "usart.h"

namespace hitcon {
namespace service {
namespace xboard {

class XBoardService {
public:
    XBoardService();

    void Init();

    // Append the data for transmit.
    void QueueDataForTx(uint8_t* data, size_t len);

    // Whenever a byte is received, this will be called.
    void SetOnByteRx(callback_t callback, void* callback_arg1);

    void NotifyTxFinish() {
        tx_busy_ = false;
    }

    void NotifyRxFinish();

    static constexpr size_t kTxBufferSize = 128;
   private:
    // Internal service routine.
    void Routine(void*);

    void TriggerRx();

    void OnRxWrapper(void *arg2);

    UART_HandleTypeDef huart_;

    callback_t on_rx_callback_;
    void* on_rx_callback_arg1_;

    bool tx_busy_ = false;

    bool rx_task_busy_ = false;
    hitcon::service::sched::Task rx_task_;

    uint8_t rx_byte_;

    uint8_t tx_buffer_[kTxBufferSize];
    // Next byte to be written to hardware.
    int tx_buffer_head_ = 0;
    // Next byte from the upper layer.
    int tx_buffer_tail_ = 0;

};

extern XBoardService g_xboard_service;

}  // namespace xboard
}  // namespace service
}  // namespace hitcon

#endif // #ifndef HITCON_SERVICE_XBOARD_SERVICE_H_
