# multicontrastLEDcontrol
Arduino-based controller for an LED multicontrast light head.

## What
A bright, 11x14-inch LED light source which uses separate green and ultraviolet light channels to enable [split filter printing](http://www.camramirez.com/pdf/P1_SplitFilterPrinting.pdf) techniques for contact printing with [paper negatives](http://www.ilfordphoto.com/aboutus/page.asp?n=151). The control unit allows separate timing and power settings for each channel.

## Why?!
Where to start? I have recently rekindled my interest in chemical-based photography. My makeshift basement darkroom is too dusty and full of light leaks for processing panchromatic film, so I shoot photo paper instead. I use large format cameras and make contact prints of my paper negatives. The opaque base material of a paper negative transmits two stops less light than transparent film base, so I need a bright light source for printing. No manufacturers make a bright LED light source for contact printing. So, I made my own.

## Inspirations
- [LED enlarger light heads from Heiland Electronic](http://heilandelectronic.de/led_kaltlicht/lang:en) 
- [Designing and Building an LED VC Enlarger Head](http://www.jbhphoto.com/wp-content/uploads/2015/01/LEDsVCPrinting03.pdf) from JB Harlin

## Parts List
- 1x 24v power supply
- 3x [FemtoBuck LED Driver](https://www.sparkfun.com/products/13716)
- 10x [UV 3w LED 365nm](https://www.amazon.com/gp/product/B00OFR96XG/ref=oh_aui_detailpage_o03_s00?ie=UTF8&psc=1)
- 2x [Green 3w LED 520-530nm](https://www.amazon.com/gp/product/B00MTGDA9Q/ref=oh_aui_detailpage_o03_s00?ie=UTF8&psc=1)
- Thermal paste
- 1x Arduino UNO
- 1x Arduino shield
- 1x [Rotary Encoder](https://www.sparkfun.com/products/10982)
- 1x [SparkFun 7-Segment Serial Display - Red](https://www.sparkfun.com/products/11441)
- 4x surplus switch and an LED assembly 
- 1x 8x16-inch sheet steel
- Wooden/cardboard structure
- Various screws/nuts/bolts
- 4-conductor phone wire
