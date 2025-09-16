import via.cli;

import std;

auto main(int argc, char** argv) -> int {
    auto app = CliApplication { argc, argv };

    return app.exec();
}
