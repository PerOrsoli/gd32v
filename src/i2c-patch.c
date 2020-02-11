// 200106
// copied from GD32V driver, temporary solution

#include <stdint.h>
#include "i2c-patch.h"
#include "gd32vf103_rcu.h"

#define I2C_CTL1(i2cx)                REG32((i2cx) + 0x04U)      /*!< I2C control register 1 */
#define I2C_CKCFG(i2cx)               REG32((i2cx) + 0x1CU)      /*!< I2C clock configure register */
#define I2C_RT(i2cx)                  REG32((i2cx) + 0x20U)      /*!< I2C rise time register */

#define I2CCLK_MAX                    ((uint32_t)0x00000048U)             /*!< i2cclk maximum value */
#define I2CCLK_MIN                    ((uint32_t)0x00000002U)             /*!< i2cclk minimum value */
#define I2C_CTL1_I2CCLK               BITS(0,5)     /*!< I2CCLK[5:0] bits (peripheral clock frequency) */
#define I2C_CKCFG_CLKC                BITS(0,11)    /*!< clock control register in fast/standard mode (master mode) */
#define I2C_DTCY_2                    ((uint32_t)0x00000000U)                  /*!< I2C fast mode Tlow/Thigh = 2 */
#define I2C_CKCFG_DTCY                BIT(14)       /*!< fast mode duty cycle */
#define I2C_CKCFG_FAST                BIT(15)       /*!< I2C speed selection in master mode */

void i2c_clock_config2(uint32_t i2c_periph, uint32_t clkspeed, uint32_t dutycyc)
{
	uint32_t pclk1, clkc, freq, risetime;
	uint32_t temp;

	pclk1 = rcu_clock_freq_get(CK_APB1);
	/* I2C peripheral clock frequency */
	freq = (uint32_t) (pclk1 / 1000000U);
	if (freq >= I2CCLK_MAX) {
		freq = I2CCLK_MAX;
	}
	temp = I2C_CTL1(i2c_periph);
	temp &= ~I2C_CTL1_I2CCLK;
	temp |= freq;

	I2C_CTL1(i2c_periph) = temp;

	if (100000U >= clkspeed) {
		/* the maximum SCL rise time is 1000ns in standard mode */
		risetime = (uint32_t) ((pclk1 / 1000000U) + 1U);
		if (risetime >= I2CCLK_MAX) {
			I2C_RT(i2c_periph) = I2CCLK_MAX;
		} else if (risetime <= I2CCLK_MIN) {
			I2C_RT(i2c_periph) = I2CCLK_MIN;
		} else {
			I2C_RT(i2c_periph) = risetime;
		}
		clkc = (uint32_t) (pclk1 / (clkspeed * 2U));
		if (clkc < 0x04U) {
			/* the CLKC in standard mode minmum value is 4 */
			clkc = 0x04U;
		}
		I2C_CKCFG(i2c_periph) |= (I2C_CKCFG_CLKC & clkc);

	} else if (400000U >= clkspeed) {
		/* the maximum SCL rise time is 300ns in fast mode */
		I2C_RT(i2c_periph) = (uint32_t) (((freq * (uint32_t) 300U)
					/ (uint32_t) 1000U) + (uint32_t) 1U);
		if (I2C_DTCY_2 == dutycyc) {
			/* I2C duty cycle is 2 */
			clkc = (uint32_t) (pclk1 / (clkspeed * 3U));
			I2C_CKCFG(i2c_periph) &= ~I2C_CKCFG_DTCY;
		} else {
			/* I2C duty cycle is 16/9 */
			clkc = (uint32_t) (pclk1 / (clkspeed * 25U));
			I2C_CKCFG(i2c_periph) |= I2C_CKCFG_DTCY;
		}
		if (0U == (clkc & I2C_CKCFG_CLKC)) {
			/* the CLKC in fast mode minmum value is 1 */
			clkc |= 0x0001U;
		}
		I2C_CKCFG(i2c_periph) |= I2C_CKCFG_FAST;
		I2C_CKCFG(i2c_periph) |= clkc;
	} else {

	}
}
