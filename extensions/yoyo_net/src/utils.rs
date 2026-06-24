pub type YResult<T> = Result<T, String>;

#[macro_export]
macro_rules! unwrap_result {
    ($result:expr) => {
        match $result {
            Ok(val) => val,
            Err(err) => {
                let error_msg = if let Some(source) = std::error::Error::source(&err) {
                    format!("{}, {}", err, source)
                } else {
                    err.to_string()
                };

                return Err(error_msg)
            }
        }
    };
}