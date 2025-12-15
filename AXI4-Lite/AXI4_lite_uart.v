module axi4lite_uart_tx (
    input         ACLK,
    input         ARESETn,

    // Write address channel
    input         AWVALID,
    output        AWREADY,
    input  [31:0] AWADDR,

    // Write data channel
    input         WVALID,
    output        WREADY,
    input  [31:0] WDATA,
    input  [3:0]  WSTRB,

    // Write response channel
    output        BVALID,
    input         BREADY,
    output [1:0]  BRESP,

    // Read address channel
    input         ARVALID,
    output        ARREADY,
    input  [31:0] ARADDR,

    // Read data channel
    output        RVALID,
    input         RREADY,
    output [31:0] RDATA,
    output [1:0]  RRESP
);

    // UART transmit register (8-bit)
    reg [7:0] tx_reg;
    reg [31:0] rdata_reg;

    // Handshake state
    reg awready_reg, wready_reg, bvalid_reg;
    reg arready_reg, rvalid_reg;

    assign AWREADY = awready_reg;
    assign WREADY  = wready_reg;
    assign BVALID  = bvalid_reg;
    assign BRESP   = 2'b00;  // OKAY

    assign ARREADY = arready_reg;
    assign RVALID  = rvalid_reg;
    assign RDATA   = rdata_reg;
    assign RRESP   = 2'b00;  // OKAY

    // Write Logic
    always @(posedge ACLK) begin
        if (!ARESETn) begin
            awready_reg <= 0;
            wready_reg  <= 0;
            bvalid_reg  <= 0;
            tx_reg      <= 8'd0;
        end else begin
            // Address ready
            if (AWVALID && !awready_reg)
                awready_reg <= 1;
            else
                awready_reg <= 0;

            // Data ready
            if (WVALID && !wready_reg)
                wready_reg <= 1;
            else
                wready_reg <= 0;

            // Write register
            if (AWVALID && WVALID && awready_reg && wready_reg) begin
                if (WSTRB[0])  // byte-enable for lowest byte
                    tx_reg <= WDATA[7:0];
                bvalid_reg <= 1;
            end else if (BVALID && BREADY) begin
                bvalid_reg <= 0;
            end
        end
    end

    // Read Logic
    always @(posedge ACLK) begin
        if (!ARESETn) begin
            arready_reg <= 0;
            rvalid_reg  <= 0;
            rdata_reg   <= 32'd0;
        end else begin
            if (ARVALID && !arready_reg)
                arready_reg <= 1;
            else
                arready_reg <= 0;

            if (ARVALID && arready_reg && !rvalid_reg) begin
                rdata_reg  <= {24'd0, tx_reg};  // Read back the 8-bit tx_reg
                rvalid_reg <= 1;
            end else if (RVALID && RREADY) begin
                rvalid_reg <= 0;
            end
        end
    end

endmodule
