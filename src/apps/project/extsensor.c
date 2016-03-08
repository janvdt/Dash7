/* * OSS-7 - An opensource implementation of the DASH7 Alliance Protocol for ultra
 * lowpower wireless sensor communication
 *
 * Copyright 2015 University of Antwerp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hwsystem.h"
#include "hwadc.h"
#include "em_system.h"
#include "em_emu.h"
#include "em_cmu.h"
#include <debug.h>

// Factory calibration temperature (from device information page)
#define CAL_TEMP_0 (float)((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >> _DEVINFO_CAL_TEMP_SHIFT)

// Factory ADC readout at CAL_TEMP_0 temperature (from device information page)
#define ADC_TEMP_0_READ_1V25 (float)((DEVINFO->ADC0CAL2 & _DEVINFO_ADC0CAL2_TEMP1V25_MASK) >> _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT)

// temperature gradient (from datasheet)
#define T_GRAD -4.5f

float get_external_temperature()
{
  adc_init(adcReference1V25, adcInputSingleCh4, 400000);

  // TODO take into account warmup time
  uint32_t value = adc_read_single();

  // TODO formula for conversion
  return (value/100);
  //return value;
}



