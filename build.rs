
// /// Build the yoyo core. core/ folder
// fn build_yoyo(system_headers_path: &PathBuf) {
//     let mut build = cc::Build::new();
//     build.warnings(false);

//     // Add sources
//     build.file("core/yoyo.cpp");
//     // Add headers
//     build.include("core/");
    
//     // Also add PXS includes as system <> headers
//     // Link the static binaries 
// }

fn main() {
    println!("cargo:rerun-if-changed=build.rs"); 
    println!("cargo:rerun-if-changed=core");
    let target = std::env::var("TARGET").unwrap();
    // Forward target to binary 
    println!("cargo:rustc-env=TARGET={}", target);

    // Lets build yoyo!
    let mut build = cc::Build::new();
    build.warnings(false);
    build.cpp(true);
    build.std("c++17");

    // Add sources
    build.file("core/yoyo.cpp");
    // Header
    build.include("core/");
    
    // Add PXS headers
    if target.contains("msvc") {
        build.flag("/external:W0");
        build.flag("/external:Ipixelscript"); 
    } else {
        build.flag("-isystem");
        build.flag("pixelscript");
    }
    // For the yoyo binary only.
    build.define("YOYO_APP", None);

    // STDLIB
    build.define("YOYO_CORE", None);

    let profile = std::env::var("PROFILE").unwrap();
    if profile == "debug" {
        build.define("YOYO_DEBUG", None);
    }

    // compile
    build.compile("yoyo_core");

    // Link libs
    println!("cargo:rustc-link-search=native=pixelscript/pxsb");
    println!("cargo:rustc-link-lib=static=pixelscript");
    println!("cargo:rustc-link-lib=static=lua");
    println!("cargo:rustc-link-lib=static=quickjs");
}