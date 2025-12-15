module axi4lite_interconnect (
    input         ACLK,
    input         ARESETn,

    /////////// Master 0 Interface////////////
	///Write address here
    input  [31:0] M0_AWADDR,
    input         M0_AWVALID,
    output        M0_AWREADY,
	
	///Write data
    input  [31:0] M0_WDATA,
    input         M0_WVALID,
    output        M0_WREADY,
	input  [3:0]  M0_WSTRB,
		
	///write response
    output        M0_BVALID,
    input         M0_BREADY,
	output [1:0]  M0_BRESP,
	
	//////Read address//
    input  [31:0] M0_ARADDR,
    input         M0_ARVALID,
    output        M0_ARREADY,
	
	///read data//
    output [31:0] M0_RDATA,
    output        M0_RVALID,
    input         M0_RREADY,
	output [1:0]  M0_RRESP,

    /////////// Master 1 Interface/////////
    input  [31:0] M1_AWADDR,
    input         M1_AWVALID,
    output        M1_AWREADY,
	
    input  [31:0] M1_WDATA,
    input         M1_WVALID,
    output        M1_WREADY,
	input  [3:0]  M1_WSTRB,
	
    output        M1_BVALID,
    input         M1_BREADY,
	output [1:0]  M1_BRESP,
	
    input  [31:0] M1_ARADDR,
    input         M1_ARVALID,
    output        M1_ARREADY,
	
    output [31:0] M1_RDATA,
    output        M1_RVALID,
    input         M1_RREADY,
	output [1:0]  M1_RRESP,

    // Slave 0 (RAM)
    output [31:0] S0_AWADDR,
    output        S0_AWVALID,
    input         S0_AWREADY,
	
    output [31:0] S0_WDATA,
    output        S0_WVALID,
    input         S0_WREADY,
	output [3:0]  S0_WSTRB,
	
    input         S0_BVALID,
	input [1:0]   S0_BRESP,
    output        S0_BREADY,
	
    output [31:0] S0_ARADDR,
    output        S0_ARVALID,
    input         S0_ARREADY,
	
    input  [31:0] S0_RDATA,
    input         S0_RVALID,
    output        S0_RREADY,
	input [1:0]   S0_RRESP,

    // Slave 1 (ROM)
    //output [31:0] S1_AWADDR,
    //output        S1_AWVALID,
   // input         S1_AWREADY,
	
    //output [31:0] S1_WDATA,
    //output        S1_WVALID,
    //input         S1_WREADY,
	
    //input         S1_BVALID,
	//input [1:0]   S1_BRESP,
    //output        S1_BREADY,
	
    output [31:0] S1_ARADDR,
    output        S1_ARVALID,
    input         S1_ARREADY,
	
    input  [31:0] S1_RDATA,
    input         S1_RVALID,
    output        S1_RREADY,
	input [1:0]   S1_RRESP
);

    // Master to slave address decoding--M0 will target either S) or S1//
    wire m0_sel_s0_w = (M0_AWADDR[12] == 1'b0); //////write into slave
    wire m0_sel_s1_w = (M0_AWADDR[12] == 1'b1);
    wire m1_sel_s0_w = (M1_AWADDR[12] == 1'b0);
    wire m1_sel_s1_w = (M1_AWADDR[12] == 1'b1);

    wire m0_sel_s0_r = (M0_ARADDR[12] == 1'b0);////////read from salve
    wire m0_sel_s1_r = (M0_ARADDR[12] == 1'b1);
    wire m1_sel_s0_r = (M1_ARADDR[12] == 1'b0);
    wire m1_sel_s1_r = (M1_ARADDR[12] == 1'b1);

    ////////////////////////// WRITE ADDRESS //////////////////////////
    assign S0_AWADDR  = m0_sel_s0_w ? M0_AWADDR : (m1_sel_s0_w ? M1_AWADDR : 32'h0);
    assign S0_AWVALID = m0_sel_s0_w ? M0_AWVALID : (m1_sel_s0_w ? M1_AWVALID : 1'b0);
    //assign S1_AWADDR  = m0_sel_s1_w ? M0_AWADDR : (m1_sel_s1_w ? M1_AWADDR : 32'h0);
    //assign S1_AWVALID = m0_sel_s1_w ? M0_AWVALID : (m1_sel_s1_w ? M1_AWVALID : 1'b0);

    assign M0_AWREADY = m0_sel_s0_w ? S0_AWREADY : 1'b0;
    assign M1_AWREADY = m1_sel_s0_w ? S0_AWREADY : 1'b0;

    ////////////////////////// WRITE DATA //////////////////////////
    assign S0_WDATA  = m0_sel_s0_w ? M0_WDATA : (m1_sel_s0_w ? M1_WDATA : 32'h0);
    assign S0_WVALID = m0_sel_s0_w ? M0_WVALID : (m1_sel_s0_w ? M1_WVALID : 1'b0);
    //assign S1_WDATA  = m0_sel_s1_w ? M0_WDATA : (m1_sel_s1_w ? M1_WDATA : 32'h0);
    //assign S1_WVALID = m0_sel_s1_w ? M0_WVALID : (m1_sel_s1_w ? M1_WVALID : 1'b0);
	
	assign S0_WSTRB = m0_sel_s0_w ? M0_WSTRB : (m1_sel_s0_w ? M1_WSTRB : 4'b0000);

    assign M0_WREADY = m0_sel_s0_w ? S0_WREADY :1'b0;
    assign M1_WREADY = m1_sel_s0_w ? S0_WREADY :1'b0;
	

    ////////////////////////// WRITE RESPONSE //////////////////////////
   // assign M0_BVALID = m0_sel_s0_w ? S0_BVALID : (m0_sel_s1_w ? S1_BVALID : 1'b0);
    //assign M1_BVALID = m1_sel_s0_w ? S0_BVALID : (m1_sel_s1_w ? S1_BVALID : 1'b0);

    //assign S0_BREADY = m0_sel_s0_w ? M0_BREADY : (m1_sel_s0_w ? M1_BREADY : 1'b0);
    //assign S1_BREADY = m0_sel_s1_w ? M0_BREADY : (m1_sel_s1_w ? M1_BREADY : 1'b0);
	
	//assign M0_BRESP = m0_sel_s0_w ? S0_BRESP : (m0_sel_s1_w ? S1_BRESP : 2'b00);
    //assign M1_BRESP = m1_sel_s0_w ? S0_BRESP : (m1_sel_s1_w ? S1_BRESP : 2'b00);


    ////////////////////////// READ ADDRESS //////////////////////////
    assign S0_ARADDR  = m0_sel_s0_r ? M0_ARADDR : (m1_sel_s0_r ? M1_ARADDR : 32'h0);
    assign S0_ARVALID = m0_sel_s0_r ? M0_ARVALID : (m1_sel_s0_r ? M1_ARVALID : 1'b0);
    assign S1_ARADDR  = m0_sel_s1_r ? M0_ARADDR : (m1_sel_s1_r ? M1_ARADDR : 32'h0);
    assign S1_ARVALID = m0_sel_s1_r ? M0_ARVALID : (m1_sel_s1_r ? M1_ARVALID : 1'b0);

    assign M0_ARREADY = m0_sel_s0_r ? S0_ARREADY : (m0_sel_s1_r ? S1_ARREADY : 1'b0);
    assign M1_ARREADY = m1_sel_s0_r ? S0_ARREADY : (m1_sel_s1_r ? S1_ARREADY : 1'b0);

    ////////////////////////// READ DATA //////////////////////////
    assign M0_RDATA  = m0_sel_s0_r ? S0_RDATA : (m0_sel_s1_r ? S1_RDATA : 32'h0);
    assign M0_RVALID = m0_sel_s0_r ? S0_RVALID : (m0_sel_s1_r ? S1_RVALID : 1'b0);
    assign M1_RDATA  = m1_sel_s0_r ? S0_RDATA : (m1_sel_s1_r ? S1_RDATA : 32'h0);
    assign M1_RVALID = m1_sel_s0_r ? S0_RVALID : (m1_sel_s1_r ? S1_RVALID : 1'b0);

    assign S0_RREADY = m0_sel_s0_r ? M0_RREADY : (m1_sel_s0_r ? M1_RREADY : 1'b0);
    assign S1_RREADY = m0_sel_s1_r ? M0_RREADY : (m1_sel_s1_r ? M1_RREADY : 1'b0);
	
    assign M0_RRESP = m0_sel_s0_r ? S0_RRESP : (m0_sel_s1_r ? S1_RRESP : 2'b00);
    assign M1_RRESP = m1_sel_s0_r ? S0_RRESP : (m1_sel_s1_r ? S1_RRESP : 2'b00);
	
endmodule
