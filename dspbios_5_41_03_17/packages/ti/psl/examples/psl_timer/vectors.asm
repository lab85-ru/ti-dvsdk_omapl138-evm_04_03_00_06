
	.sect .vectors
	
	.ref _Timer0Isr
	
	.ivec _UnusedIsr,USE_RETA
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _Timer0Isr  ; timer0 isr
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	.ivec _UnusedIsr  
	
	.sect ".text"
	
	.global _UnusedIsr
_UnusedIsr  B _UnusedIsr

	.end
	