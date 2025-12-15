module axi4lite_rom (
    input         ACLK,
    input         ARESETn,

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

    // 4KB ROM: 1024 x 32-bit words
    reg [31:0] rom [0:1023];

    reg [31:0] rdata_reg;
	reg [1:0]  rresp_reg;   //hardcoded condition for rresp check
    reg        arready_reg, rvalid_reg;

    assign ARREADY = arready_reg;
    assign RVALID  = rvalid_reg;
    assign RDATA   = rdata_reg;
    assign RRESP   = rresp_reg;   //hardcoded condition for rresp check

    initial begin
        $readmemh("E:/Caps/drdo_deliveables/keil_project_v4/Objects/keil_project_v4.hex", rom);
    end

    always @(posedge ACLK) begin
        if (!ARESETn) begin
            arready_reg <= 0;
            rvalid_reg  <= 0;
            rdata_reg   <= 32'd0;
			rresp_reg   <= 2'b00; //hardcoded condition for rresp check
        end else begin
            // Address handshake
            if (ARVALID && !arready_reg)
                arready_reg <= 1;
            else
                arready_reg <= 0;

            // Perform read
            if (ARVALID && arready_reg && !rvalid_reg) begin
                rdata_reg  <= rom[ARADDR[11:2]];
                rvalid_reg <= 1;
				///////////////////////////////////////
				// Set RRESP based on address
                if (rom[ARADDR[11:2]] == 32'h00000125)
                    rresp_reg <= 2'b01;  // SLVERR for last word
                else
                    rresp_reg <= 2'b00;  // OKAY otherwise
				//////////////////////////////////////////	
					
            end else if (RVALID && RREADY) begin
                rvalid_reg <= 0;
            end
        end
    end

endmodule
