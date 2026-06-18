use std::path::PathBuf;

use crate::commands::{ArgParser, Context, pxs, repl, run};

mod yoyo;
mod commands;
mod utils;

const USAGE: &str = r#"YoYo v0.0.1
-h,  --help          Get this message.
-py, --python        Set the python runtime for compiling pixelscript.
     --target        The compilation target for compiling pixelscript.

pxs <--python> <--target> Compile pixelscript for a specific target (or build) and pass in the path or env name for python runtime.
"#;


fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut context = Context::new();

    if args.len() < 2 {
        println!("{USAGE}");
        return;
    }

    let mut arg_parser = ArgParser::new(args);
    arg_parser.parse();
    // skip yoyo
    arg_parser.next_arg();

    let cwd = arg_parser.get_flag_keys(vec!["--path", "-p"]);
    if let Some(cwd) = cwd {
        context.set_cwd(PathBuf::from(cwd.get_value()));
    } else {
        context.set_cwd(std::env::current_dir().unwrap());
    }

    let python = arg_parser.get_flag_keys(vec!["--python", "-py"]);
    if let Some(python) = python {
        context.python_name = python.get_value().clone();
    }

    let target = arg_parser.get_flag_keys(vec!["--target"]);
    if let Some(target) = target {
        context.target = target.get_value().clone();
    } else {
        context.target = env!("TARGET").to_string();
    }

    let command = arg_parser.next_arg();
    if let Some(command) = command {
        let result = if command == "pxs" {
            pxs(&context, &mut arg_parser)
        } else if command == "repl" {
            repl(&mut arg_parser)
        } else {
            // Maybe this is a `run` command?
            run(&context, &mut arg_parser)
        };

        match result {
            Ok(_) => {},
            Err(err) => println!("{err}"),
        }
    } else {
        println!("{USAGE}");
    }
}

