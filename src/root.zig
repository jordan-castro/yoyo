//! The API for working with yoyo without it's runtime.
//! Think "yoyo.os", "yoyo.io", "yoyo.cross" Without being forced to use Yoyo.
const std = @import("std");
const Io = std.Io;
const Allocator = std.mem.Allocator;

pub const Version = struct {
    pub const major: u32 = 0;
    pub const minor: u32 = 0;
    pub const patch: u32 = 1;

    pub fn to_string(allocator: std.mem.Allocator) ![]u8 {
        const result = try allocator.alloc(u8, 5);
        _ = try std.fmt.bufPrint(result, "{}.{}.{}", .{Version.major, Version.minor, Version.patch});
        
        return result;
    }
};

pub const PIXEL_SCRIPT_ZIP_URL = "https://github.com/jordan-castro/pixelscript/archive/refs/heads/master.zip";
pub const YOYO_DIR = ".yoyo";
pub const PIXEL_SCRIPT_ZIP_PATH = "pixelscript.zip";

/// Download pixelscript zip file.
/// 
/// Returns `yoyo_dir`.
pub fn download_pixelscript(io: Io, allocator: Allocator, cwd: ?Io.Dir) !Io.Dir {
    var client = std.http.Client{
        .allocator = allocator,
        .io = io
    };
    defer client.deinit();

    // Open the file to save pixelscript zip to
    const yoyo_dir = try get_yoyo_dir(io, cwd);

    var file = try yoyo_dir.createFile(io, "pixelscript.zip", .{.read = true});
    defer file.close(io);

    var download_buffer: [64 * 1024]u8 = undefined;
    // Create file writer
    var file_writer = file.writer(io, &download_buffer);

    // Create URI
    const uri = try std.Uri.parse(PIXEL_SCRIPT_ZIP_URL);

    // Make the HTTP request
    _ = try client.fetch(.{
        .location = .{.uri = uri},
        .method = .GET,
        .extra_headers = &.{},
        .headers = .{},
        .response_writer = &file_writer.interface
    });
}

/// Extract pixelscript zip file.
pub fn extract_pixelscript(io: Io, allocator: Allocator, yoyo_dir: Io.Dir) !void {
    var zip_file = try yoyo_dir.openFile(io, PIXEL_SCRIPT_ZIP_PATH, .{.mode = .read_only});
    defer zip_file.close(io);

    try yoyo_dir.createDirPath(io, ".pixelscript/", .{});
    const zip_dir = try std.Io.Dir.openDir(yoyo_dir, io, ".pixelscript/", .{});

    const buffer: [64 * 1024]u8 = undefined;
    var stream = zip_file.reader(io, &buffer);
    
    var zip_archive = try std.zip.Iterator.init(&stream);

    while (try zip_archive.next()) |entry| {
        if (entry.)
    }
}

/// Get the .yoyo directory
pub fn get_yoyo_dir(io: Io, passed: ?Io.Dir) !std.Io.Dir {
    // Get CWD
    var cwd = passed orelse std.Io.Dir.cwd();
    try cwd.createDirPath(io, ".yoyo");
    const yoyo_dir = try std.Io.Dir.openDir(cwd, io, ".yoyo/", .{});
    return yoyo_dir;
}