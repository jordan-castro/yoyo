const std = @import("std");

const yoyo = @import("yoyo");

pub fn main(init: std.process.Init) !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();

    const allocator = arena.allocator();

    std.debug.print("YoYo {s}!", .{try yoyo.Version.to_string(allocator)});

    // Get commands
    var command_args = try std.process.Args.iterateAllocator(init.minimal.args, allocator);
    // Skip arg #1 (yoyo)
    _ = command_args.skip();

    // Get the command
    const command = command_args.next() orelse {
        return;
    };
    // Let's check what it IS
    if (std.mem.eql(u8, command, "start")) {
        // Lets start here!
        try start(init.io, allocator);
    }
}

fn start(io: std.Io, allocator: std.mem.Allocator, passed_dir: ?std.Io.Dir) !void {
    const yoyo_dir = try yoyo.download_pixelscript(io, allocator, passed_dir);
    _ = try yoyo.extract_pixelscript(io, allocator, yoyo_dir);
}