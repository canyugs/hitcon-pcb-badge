/*
 * LedMatrix.cc
 *
 *  Created on: Apr 13, 2024
 *      Author: doraeric
 */

#include "LedMatrix.h"
#include "main.h"

namespace hitcon {

static GPIO_TypeDef *cPorts[] = {LedCa_GPIO_Port, LedCb_GPIO_Port, LedCc_GPIO_Port, LedCd_GPIO_Port, LedCe_GPIO_Port, LedCf_GPIO_Port, LedCg_GPIO_Port, LedCh_GPIO_Port};
static GPIO_TypeDef *rPorts[] = {LedRa_GPIO_Port, LedRb_GPIO_Port, LedRc_GPIO_Port, LedRd_GPIO_Port, LedRe_GPIO_Port, LedRf_GPIO_Port, LedRg_GPIO_Port, LedRh_GPIO_Port};
static uint16_t cPins[] = {LedCa_Pin, LedCb_Pin, LedCc_Pin, LedCd_Pin, LedCe_Pin, LedCf_Pin, LedCg_Pin, LedCh_Pin};
static uint16_t rPins[] = {LedRa_Pin, LedRb_Pin, LedRc_Pin, LedRd_Pin, LedRe_Pin, LedRf_Pin, LedRg_Pin, LedRh_Pin};

static bool matrix[8][8] = {0};
const static uint8_t NB_ROW = 8, NB_COL = 8;
static uint8_t rid = NB_ROW-1;

LedMatrix::LedMatrix() {
	// TODO Auto-generated constructor stub
//	matrix[1][1] = true;
//	matrix[1][6] = true;
//	matrix[4]
	for (uint8_t r=0; r<NB_ROW; ++r) {
		for (uint8_t c=0; c<NB_COL; ++c) {
			matrix[r][c] = true;
		}
	}
	matrix[0][0] = true;
}

LedMatrix::~LedMatrix() {
	// TODO Auto-generated destructor stub
}

void LedMatrix::update() {
	// reset previous row
	HAL_GPIO_WritePin(rPorts[rid], rPins[rid], GPIO_PIN_RESET);

	rid = (rid+1) % NB_ROW;
	HAL_GPIO_WritePin(rPorts[rid], rPins[rid], GPIO_PIN_SET);
	for (uint8_t cid=0; cid<NB_COL; ++cid) {
		auto pin_state = matrix[rid][cid] ? GPIO_PIN_SET : GPIO_PIN_RESET;
		HAL_GPIO_WritePin(cPorts[cid], cPins[cid], pin_state);
	}
}

void LedMatrix::reset_current_row() {
	HAL_GPIO_WritePin(rPorts[rid], rPins[rid], GPIO_PIN_RESET);
	for (uint8_t cid=0; cid<NB_COL; ++cid) {
		HAL_GPIO_WritePin(cPorts[cid], cPins[cid], GPIO_PIN_RESET);
	}
}

void LedMatrix::trigger() {
	// if (HAL_DMA_PollForTransfer())
	update();
//	HAL_Delay(4);
	for (int i=0; i<1; ++i);
	reset_current_row();
//	HAL_Delay(1);
	for (int i=0; i<500; ++i);
}

} /* namespace hitcon */
