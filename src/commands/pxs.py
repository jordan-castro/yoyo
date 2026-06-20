# pub fn pxs(context: &Context, arg_parser: &mut ArgParser) -> YoyoResult {
#     let mut current_dir = context.path_buf().clone();
#     let should_clear = match arg_parser.next_arg() {
#         Some(val) => {
#             if val == "clear" {
#                 "clear"
#             } else {
#                 ""
#             }
#         }
#         None => "",
#     };
#     // Unzip pixelscript.zip
#     let path = context.path_buf();
#     let mut path = yoyo_dir!(path);
#     utils::create_dir(&path)?;
#     utils::extract_pixelscript_zip(&path, yoyo::PIXELSCRIPT_ZIP)?;
#     path.push("pixelscript");

#     let mut script_path = path.clone();
#     script_path.push("scripts");
#     // Change directories
#     get_result!(std::env::set_current_dir(&path))?;
#     script_path.push("build.py");

#     let status = Command::new(&context.python_name)
#         .arg(script_path)
#         .arg(should_clear)
#         .status();
#     let _ = get_result!(status)?;

#     // Change back to working directory
#     get_result!(std::env::set_current_dir(&current_dir))?;

#     path.push("pxsb");
#     // Save the binaries into the current target
#     current_dir.push("pxsb");
#     current_dir.push(&context.target);
#     utils::create_dir(&current_dir)?;

#     // Get files from pxsb
#     let files = get_result!(std::fs::read_dir(&path))?;
#     for f in files {
#         let f = get_result!(f)?;
#         let mut new_path = current_dir.clone();
#         new_path.push(f.file_name());
#         get_result!(std::fs::rename(f.path(), &new_path))?;
#     }

#     println!(
#         "pixelscript binaries have been written to {}",
#         current_dir.display()
#     );
#     Ok(())
# }
