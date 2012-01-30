module vga_top(
	output reg	h_sync,
	output reg	v_sync,
	output reg [7:0]	color,
	
	input nwe, nwe1, ncs, //No chip select No output enable
	input [7:0] 	sram_data,
	input [12:0] 	addr_pr,
	input	clk,
	output	led,
	input	rst
);
	parameter MEM_ADDR_WIDTH = 14;
	parameter MEM_DATA_WIDTH = 1;
	 
	// synchronize signals                               
	reg	sncs, snwe, snwe1;
	reg	[12:0] buffer_addr_wr;
	reg	[MEM_DATA_WIDTH-1:0] buffer_data;

	// interfaz fpga signals
	// wire   [12:0] addr;
	
	// bram interface signals
	reg	[MEM_DATA_WIDTH-1:0]	wdBus;
	wire	[MEM_DATA_WIDTH-1:0]	rdBus;
	reg	[MEM_ADDR_WIDTH-1:0]	addr_rd;
	reg	[MEM_ADDR_WIDTH-1:0]	addr_wr;
	reg	mem_we;
	reg	rst_addr_rd_count;

	//control vga
	wire	blank; //define si se encuentra en región de dibujo o fuera de esta
	reg  [1:0]	clk2;
	wire [10:0]	hcounter;
	wire [10:0]	vcounter;
	
	initial clk2 = 0;
	initial color = 'b00000011;
	initial addr_rd = 0;
	
	//--------------------------------------------------------------------------
	// interfaz vga - memoria y definicion de color
	//--------------------------------------------------------------------------
	
	//lectura desde la memoria para desplegar info en vga con clk 25 MHz
	always @(posedge clk2[1]) begin
		addr_rd <= addr_rd;
		color <= color;
		if (~rst) begin
			color <= 0;
			addr_rd <= 0;
			rst_addr_rd_count <= 1;
		end
		
		else if(!blank)	begin			
			if (rdBus)	color <= 8'hFF;
			else 		color <= 8'b00;
			
			rst_addr_rd_count <= 0;
		end
		
		else begin
			if (vcounter == 480)	rst_addr_rd_count <= 1;
		end
	end
	
	
	always @(posedge clk) begin
		clk2 <= clk2 + 1;
	end
	

	
	vga_controller_640_60 ctrl0 (
		.rst		(~rst),
		.pixel_clk	(clk),
		.HS			(h_sync),
		.VS			(v_sync),
		.hcount		(hcounter),		
		.vcount		(vcounter),
		.blank		(blank)
	);
	
	//--------------------------------------------------------------------------
	// definicion de direccion y dato para la memoria
	//--------------------------------------------------------------------------	
	/*
		El proceso de escritura de un dato en determinada dirección se hace en 
		dos ciclos del reloj del sistema
		
		Ciclo 1 addr= 10 LSB + addr[10] = 0 dato=id
		Ciclo 2 addr=  9 MSB + addr[10] = 1  dato=id
	*/
	
	always @(posedge clk) begin
		if (~rst) begin
			addr_wr <= 0;
			wdBus <= 0;
			mem_we <= 0;
		end
		
		/*	aqui se valida que este activo el chip select y we0 del procesador 
			(este último habilita la escritura en los bits [7:0] de la línea de 
			datos). Tambien se valida que no esté activo we1, que habilita la
			escritura en los bits [15:8] de la línea de datos		
		*/
		else if (~sncs & ~snwe & snwe1)begin
			if (buffer_addr_wr[10] == 0) begin
				wdBus <= wdBus;
				addr_wr[9:0] <= buffer_addr_wr[9:0];
				mem_we <= 0;
			end
			
			else begin
				wdBus <= buffer_data[MEM_DATA_WIDTH-1:0];
				addr_wr[13:10] <= buffer_addr_wr[8:0];
				mem_we <= 1;
			end
		end
		
		else begin
			wdBus <= wdBus;
			mem_we <= 0;
			addr_wr <= addr_wr;
		end
		
	end
	
	
	// synchronize assignment
	always  @(negedge clk) begin
		sncs   <= ncs;
		snwe   <= nwe;
		snwe1  <= nwe1;
		buffer_data <= sram_data;
		buffer_addr_wr <= addr_pr;
	end

	//Verifica que el archivo de configuración esté cargado
	reg [24:0]  counter;
	
	always @(posedge clk) begin
		if (~rst)
			counter <= {25{1'b0}};
		else
			counter <= counter + 1;
	end
	
	assign led = counter[24];              
  
endmodule
