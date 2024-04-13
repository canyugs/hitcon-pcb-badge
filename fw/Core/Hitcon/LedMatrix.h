/*
 * LedMatrix.h
 *
 *  Created on: Apr 13, 2024
 *      Author: doraeric
 */

#ifndef HITCON_LEDMATRIX_H_
#define HITCON_LEDMATRIX_H_

namespace hitcon {

class LedMatrix {
public:
	LedMatrix();
	virtual ~LedMatrix();
	void update();
	void trigger();
	void reset_current_row();
};

} /* namespace hitcon */

#endif /* HITCON_LEDMATRIX_H_ */
