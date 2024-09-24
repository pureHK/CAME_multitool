#include "math_calculations.h"
#include "data.h"


uint16_t math_gate_len_to_imp( uint16_t gate_length ) {
	uint16_t impulses = 1250UL * gd->calibration_data.imp_per_rotation * gd->calibration_data.worm_gear_teeth_count /
						3927UL * gate_length / gd->calibration_data.gear_module / (gd->calibration_data.gear_teeth_count + 2);

	return impulses;
}
