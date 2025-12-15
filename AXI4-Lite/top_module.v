module top_module (
    input wire clk,
    input wire resetn
);

    // AXI4-Lite Master interface from PicoRV32
    wire [31:0] m_axi_awaddr, m_axi_wdata, m_axi_araddr;
    wire [3:0]  m_axi_wstrb;
    wire        m_axi_awvalid, m_axi_awready;
    wire        m_axi_wvalid,  m_axi_wready;
    wire        m_axi_bvalid,  m_axi_bready;
    wire [1:0]  m_axi_bresp;
    wire        m_axi_arvalid, m_axi_arready;
    wire        m_axi_rvalid,  m_axi_rready;
    wire [31:0] m_axi_rdata;
    wire [1:0]  m_axi_rresp;

    // Interconnect <-> RAM signals
    wire [31:0] ram_awaddr, ram_wdata, ram_araddr;
    wire [3:0]  ram_wstrb;
    wire        ram_awvalid, ram_awready;
    wire        ram_wvalid,  ram_wready;
    wire        ram_bvalid,  ram_bready;
    wire [1:0]  ram_bresp;
    wire        ram_arvalid, ram_arready;
    wire        ram_rvalid,  ram_rready;
    wire [31:0] ram_rdata;
    wire [1:0]  ram_rresp;

    // Interconnect <-> ROM signals
   // wire [31:0] rom_awaddr, rom_wdata, rom_araddr;
   // wire [3:0]  rom_wstrb;
   // wire        rom_awvalid, rom_awready;
   // wire        rom_wvalid,  rom_wready;
   // wire        rom_bvalid,  rom_bready;
    //wire [1:0]  rom_bresp;
    wire        rom_arvalid, rom_arready;
    wire        rom_rvalid,  rom_rready;
    wire [31:0] rom_rdata, rom_araddr;
    wire [1:0]  rom_rresp;

    // Instantiate PicoRV32 AXI master core
    picorv32_axi core (
        .clk             (clk),
        .resetn          (resetn),
        .mem_axi_awaddr  (m_axi_awaddr),
        .mem_axi_awvalid (m_axi_awvalid),
        .mem_axi_awready (m_axi_awready),
        .mem_axi_wdata   (m_axi_wdata),
        .mem_axi_wstrb   (m_axi_wstrb),
        .mem_axi_wvalid  (m_axi_wvalid),
        .mem_axi_wready  (m_axi_wready),
        .mem_axi_bresp   (m_axi_bresp),
        .mem_axi_bvalid  (m_axi_bvalid),
        .mem_axi_bready  (m_axi_bready),
        .mem_axi_araddr  (m_axi_araddr),
        .mem_axi_arvalid (m_axi_arvalid),
        .mem_axi_arready (m_axi_arready),
        .mem_axi_rdata   (m_axi_rdata),
        .mem_axi_rresp   (m_axi_rresp),
        .mem_axi_rvalid  (m_axi_rvalid),
        .mem_axi_rready  (m_axi_rready)
    );

    // Instantiate AXI4-Lite Interconnect (1 master, 2 slaves)
    axi4lite_interconnect axi_ic (
        .ACLK    (clk),
        .ARESETn (resetn),

        // Master 0 - PicoRV32
        .M0_AWADDR  (m_axi_awaddr),
        .M0_AWVALID (m_axi_awvalid),
        .M0_AWREADY (m_axi_awready),
        .M0_WDATA   (m_axi_wdata),
        .M0_WSTRB   (m_axi_wstrb),
        .M0_WVALID  (m_axi_wvalid),
        .M0_WREADY  (m_axi_wready),
        .M0_BVALID  (m_axi_bvalid),
        .M0_BREADY  (m_axi_bready),
        .M0_BRESP   (m_axi_bresp),
        .M0_ARADDR  (m_axi_araddr),
        .M0_ARVALID (m_axi_arvalid),
        .M0_ARREADY (m_axi_arready),
        .M0_RDATA   (m_axi_rdata),
        .M0_RVALID  (m_axi_rvalid),
        .M0_RREADY  (m_axi_rready),
        .M0_RRESP   (m_axi_rresp),

        // Master 1 - Unused
        .M1_AWADDR  (32'b0),
        .M1_AWVALID (1'b0),
        .M1_AWREADY (),
        .M1_WDATA   (32'b0),
        .M1_WSTRB   (4'b0),
        .M1_WVALID  (1'b0),
        .M1_WREADY  (),
        .M1_BVALID  (),
        .M1_BREADY  (1'b0),
        .M1_BRESP   (),
        .M1_ARADDR  (32'b0),
        .M1_ARVALID (1'b0),
        .M1_ARREADY (),
        .M1_RDATA   (),
        .M1_RVALID  (),
        .M1_RREADY  (1'b0),
        .M1_RRESP   (),

        // Slave 0 - RAM
        .S0_AWADDR  (ram_awaddr),
        .S0_AWVALID (ram_awvalid),
        .S0_AWREADY (ram_awready),
        .S0_WDATA   (ram_wdata),
        .S0_WSTRB   (ram_wstrb),
        .S0_WVALID  (ram_wvalid),
        .S0_WREADY  (ram_wready),
        .S0_BVALID  (ram_bvalid),
        .S0_BRESP   (ram_bresp),
        .S0_BREADY  (ram_bready),
        .S0_ARADDR  (ram_araddr),
        .S0_ARVALID (ram_arvalid),
        .S0_ARREADY (ram_arready),
        .S0_RDATA   (ram_rdata),
        .S0_RVALID  (ram_rvalid),
        .S0_RRESP   (ram_rresp),
        .S0_RREADY  (ram_rready),

        // Slave 1 - ROM
        //.S1_AWADDR  (rom_awaddr),
        //.S1_AWVALID (rom_awvalid),
        //.S1_AWREADY (rom_awready),
        //.S1_WDATA   (rom_wdata),
       // .S1_WVALID  (rom_wvalid),
       // .S1_WREADY  (rom_wready),
       // .S1_BVALID  (rom_bvalid),
       // .S1_BRESP   (rom_bresp),
       // .S1_BREADY  (rom_bready),
        .S1_ARADDR  (rom_araddr),
        .S1_ARVALID (rom_arvalid),
        .S1_ARREADY (rom_arready),
        .S1_RDATA   (rom_rdata),
        .S1_RVALID  (rom_rvalid),
        .S1_RRESP   (rom_rresp),
        .S1_RREADY  (rom_rready)
    );

    // Instantiate RAM (Slave 0)
    axi4lite_ram ram (
        .ACLK      (clk),
        .ARESETn   (resetn),
        .AWADDR    (ram_awaddr),
        .AWVALID   (ram_awvalid),
        .AWREADY   (ram_awready),
        .WDATA     (ram_wdata),
        .WSTRB     (ram_wstrb),
        .WVALID    (ram_wvalid),
        .WREADY    (ram_wready),
        .BVALID    (ram_bvalid),
        .BRESP     (ram_bresp),
        .BREADY    (ram_bready),
        .ARADDR    (ram_araddr),
        .ARVALID   (ram_arvalid),
        .ARREADY   (ram_arready),
        .RDATA     (ram_rdata),
        .RVALID    (ram_rvalid),
        .RRESP     (ram_rresp),
        .RREADY    (ram_rready)
    );

    // Instantiate ROM (Slave 1)
    axi4lite_rom rom (
        .ACLK      (clk),
        .ARESETn   (resetn),
        //.AWADDR    (rom_awaddr),
        //.AWVALID   (rom_awvalid),
        //.AWREADY   (rom_awready),
       // .WDATA     (rom_wdata),
       // .WVALID    (rom_wvalid),
       // .WREADY    (rom_wready),
       // .BVALID    (rom_bvalid),
       // .BRESP     (rom_bresp),
        //.BREADY    (rom_bready),
        .ARADDR    (rom_araddr),
        .ARVALID   (rom_arvalid),
        .ARREADY   (rom_arready),
        .RDATA     (rom_rdata),
        .RVALID    (rom_rvalid),
        .RRESP     (rom_rresp),
        .RREADY    (rom_rready)
    );

endmodule