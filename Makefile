mainc = main.c
daccal = daccal.c
adccal = adccal.c
blink = blinkled.c
biquad = biquad.c biquad_filter.c

def105 = -D STM32F10X_CL
defstdperiph = -D USE_STDPERIPH_DRIVER
defhse = -D 'HSE_VALUE=((uint32_t)8000000)'
defs = $(def105) $(defstdperiph) $(def8mhz)

cm3root = /usr/students/apellett/stm/arp

cmsisroot = $(cm3root)/libraries/CMSIS/CM3
stdperiphroot = $(cm3root)/libraries/STM32F10x_StdPeriph_Driver
stdperiphinc = $(stdperiphroot)/inc
stdperiphsrc = $(stdperiphroot)/src

incdevice = $(cmsisroot)/DeviceSupport/ST/STM32F10x
inccore = $(cmsisroot)/CoreSupport

startupscript = $(incdevice)/startup/gcc_ride7/startup_stm32f10x_cl.s

cm3inc = -I$(inccore) -I$(incdevice) -I$(cm3root) -I$(stdperiphinc)

miscsrc = $(stdperiphsrc)/misc.c
rccsrc = $(stdperiphsrc)/stm32f10x_rcc.c
gpiosrc = $(stdperiphsrc)/stm32f10x_gpio.c
adcsrc = $(stdperiphsrc)/stm32f10x_adc.c
dacsrc = $(stdperiphsrc)/stm32f10x_dac.c
dmasrc = $(stdperiphsrc)/stm32f10x_dma.c
timsrc = $(stdperiphsrc)/stm32f10x_tim.c
flashsrc = $(stdperiphsrc)/stm32f10x_flash.c

cfiles = $(incdevice)/*.c $(rccsrc) $(gpiosrc) $(miscsrc) $(adcsrc) \
			$(dacsrc) $(dmasrc) $(timsrc) $(flashsrc)

ccpath = /usr/students/apellett/stm/ctc/bin
CC = $(ccpath)/arm-eabi-gcc

CFLAGS = -march=armv7-m -mthumb

arpld = -T arp.ld

mainc : $(csource) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(mainc) $(startupscript) -o arp.eabi
	
biquad : $(csource) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(biquad) $(startupscript) -o arp.eabi
	
adccal : $(csource) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(adccal) $(startupscript) -o arp.eabi
	
daccal : $(csource) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(daccal) $(startupscript) -o arp.eabi
	
blink : $(csource) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(blink) $(startupscript) -o arp.eabi
	
main-all-libs : $(csource) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(stdperiphsrc)/*.c $(startupscript) -o arp.eabi.alllibs

.PHONY : clean
clean:
	rm arp.eabi arp.eabi.alllibs
	
#arm-eabi-strip