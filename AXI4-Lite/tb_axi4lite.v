`timescale 1ns/1ps

module tb_axi4lite;

    reg clk;
    reg resetn;

    // Instantiate the top-level module
    top_module dut (
        .clk(clk),
        .resetn(resetn)
    );

    // Clock generation (10 ns period => 100 MHz)
    initial clk = 0;
    always #5 clk = ~clk;

    // Reset generation
    initial begin
        resetn = 0;
        #100;
        resetn = 1;
    end

    // Load ROM contents
//    initial begin
//        $display("Loading program into ROM...");
//        $readmemh("program.hex", dut.rom_inst.mem);  // Adjust path if needed
//    end

    // Dump waveform
    initial begin
        $dumpfile("tb_axi4lite.vcd");
        $dumpvars(0, tb_axi4lite);
    end

    // Timeout
    initial begin
        #50000;
        $display("Simulation timeout.");
        $finish;
    end

endmodule
