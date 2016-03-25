////////////////////////////////////////////////////////////////////////////////////////////
//
// Example Code for ECEN 5053 Lab 5
//

#include <project.h>

// The following test parameters can be patched after code is loaded using the debug option:
uint8 chan = 0, change_chan = 0, mark_chan = 1 ;
uint16  delay_time = 1000 ;

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
	
	// "Start" the two components that require it:
	S_H_Start() ;
	MUX_Start() ;
    
    MUX_Select(2);

    // For testing purposes, this loop will cycle through the MUX input channels.
    while(1)
    {
	if( change_chan == 1)
	    {
		// Use P0_4 as a marker to know when the MUX is switched to the marked channel.
		if( chan == mark_chan )  
		   P0_4_Write(1) ;  // P0_4 is just a digital output pin.
		else 
		   P0_4_Write(0) ;
		
	    MUX_Select(chan) ;  // Change MUX channel.
		CyDelayUs( delay_time ) ;  // Delay of "delay_time" microseconds.
		
		// Cycle through the 4 multiplexer inputs.
		if( chan < 1 )  
		   chan++ ; 
		else            
		   chan = 0 ;
	    }
    }
}

/* [] END OF FILE */
