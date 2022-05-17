use tracing_subscriber::util::SubscriberInitExt;

fn main() {
    let runtime = tokio::runtime::Builder::new_multi_thread()
        .enable_all()
        .build()
        .unwrap();
    let _guard = runtime.enter();

    let options = eframe::NativeOptions::default();

    // let format = tracing_subscriber::fmt::format()
    //     .pretty()
    //     .with_ansi(false)
    //     .fmt_fields(tracing_subscriber::fmt::FormatFields::new().with_ansi(false));

    tracing_subscriber::fmt()
        .with_env_filter(tracing_subscriber::filter::EnvFilter::from_default_env())
        // .pretty()
        // .with_ansi(false)
        // .fmt_fields(tracing_subscriber::fmt)
        // .event_format(format)
        .finish()
        .init();

    eframe::run_native(
        "fmr",
        options,
        Box::new(|it| {
            it.egui_ctx.set_visuals(egui::Visuals::dark());
            Box::new(fmr::App::new(it))
        }),
    );
}
