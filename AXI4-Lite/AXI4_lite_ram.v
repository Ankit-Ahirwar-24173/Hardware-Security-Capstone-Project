module axi4lite_ram #(
    parameter ADDR_WIDTH = 12,  // 4KB = 2^12
    parameter DATA_WIDTH = 32
)(
    input                   ACLK,
    input                   ARESETn,

    // Write address channel
    input                   AWVALID,
    output                  AWREADY,
    input  [31:0]           AWADDR,

    // Write data channel
    input                   WVALID,
    output                  WREADY,
    input  [DATA_WIDTH-1:0] WDATA,
    input  [3:0]            WSTRB,

    // Write response channel
    output                  BVALID,
    input                   BREADY,
    output [1:0]            BRESP,

    // Read address channel
    input                   ARVALID,
    output                  ARREADY,
    input  [31:0]           ARADDR,

    // Read data channel
    output                  RVALID,
    input                   RREADY,
    output [DATA_WIDTH-1:0] RDATA,
    output [1:0]            RRESP
);

    // Internal memory - still used for writes (optional)
    reg [DATA_WIDTH-1:0] mem [0:(1<<ADDR_WIDTH)/4-1];

    // Internal state
    reg [DATA_WIDTH-1:0] rdata_reg;
    reg                  rvalid_reg, bvalid_reg;
    reg [DATA_WIDTH-1:0] read_counter;

    assign AWREADY = 1'b1;
    assign WREADY  = 1'b1;
    assign BVALID  = bvalid_reg;
    assign BRESP   = 2'b00;

    assign ARREADY = 1'b1;
    assign RVALID  = rvalid_reg;
    assign RDATA   = rdata_reg;
    assign RRESP   = 2'b00;

    wire [ADDR_WIDTH-1:2] write_addr = AWADDR[ADDR_WIDTH-1:2];

    integer i;
    always @(posedge ACLK) begin
        if (!ARESETn) begin
            bvalid_reg   <= 0;
            rvalid_reg   <= 0;
            read_counter <= 32'h0;
        end else begin
            // Write operation (preserved)
            if (AWVALID && WVALID && !bvalid_reg) begin
                for (i = 0; i < 4; i = i + 1) begin
                    if (WSTRB[i])
                        mem[write_addr][8*i +: 8] <= WDATA[8*i +: 8];
                end
                bvalid_reg <= 1;
            end else if (BVALID && BREADY) begin
                bvalid_reg <= 0;
            end

            // Read operation with incrementing counter value
            if (ARVALID && !rvalid_reg) begin
                rdata_reg    <= read_counter;
                read_counter <= read_counter + 32'h4;  // Increment by 4 (or any other value)
                rvalid_reg   <= 1;
            end else if (RVALID && RREADY) begin
                rvalid_reg <= 0;
            end
        end
    end

endmodule
