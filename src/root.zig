//! The API for working with yoyo without it's runtime.
//! Think "yoyo.os", "yoyo.io", "yoyo.cross" Without being forced to use Yoyo.
const std = @import("std");
const Allocator = std.mem.Allocator;

/// String type
pub const YoyoString = struct {
    contents: []u8,
    allocator: Allocator,

    /// Initialize a YoyoString with a constant string
    pub fn init_const(allocator: Allocator, str: []const u8) !YoyoString {
        const contents = try allocator.alloc([]8, str.len);
        for (str, 0..str.len) |chr, i| {
            contents.ptr[i].* = chr;
        }
        return YoyoString{
            .contents = contents,
            .allocator = allocator,
        };
    }

    pub fn push_raw(self: *YoyoString, str: []const u8) !void {
        const csize = self.contents.len;
        self.allocator.free(self.contents);
        self.contents = try self.allocator.alloc([]8, csize + str.len);
    }
};