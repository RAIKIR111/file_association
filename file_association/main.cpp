#include "essentials.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    if (argc == 1) {
        qDebug() << "Use following instrucions:";
        qDebug() << R"(To enter the configuration mode, input:)";
        qDebug() << R"(-> file_association cfg)";
        qDebug() << R"(To start the application in the "default" mode (.json data file will be configured right into your build's directory), input:)";
        qDebug() << R"(-> file_association default)";
        qDebug() << R"(To start the application in the "extended" mode, input:)";
        qDebug() << R"(-> file_association extended /path/to/your/data/file/dot/any/format)";
        qDebug() << R"(To start the application in the "temporary" mode (data file will not be created and your data will be erased after the session), input)";
        qDebug() << R"(-> file_association temporary)";
        return -1;
    }

    try {
        QString json_file_path;
        RunType run_type = GetRunType(argv[1]);

        switch (run_type)
        {
            case (RunType::DEFAULT):
                json_file_path = JSON_FILE_PATH_DEFAULT;
                return DefaultScenary(a, json_file_path);
            case (RunType::EXTENDED):
                json_file_path = argv[2];
                return ExtendedScenary(a, json_file_path);
            case (RunType::TEMPORARY):
                return TemporaryScenary(a);
            case (RunType::CFG_MODE):
                return ConfigurationScenary();
        }
    }
    catch (const IncorrectRunType& e) {
        // incorrect run type exception
        qDebug() << e.what();
        return -1;
    }
    catch (const IncorrectFormat& e) {
        // wrong data file format execption
        qDebug() << e.what();
        return -1;
    }
    catch (...) {
        qDebug() << "Exception has occured: some exception has occured before the QCoreApplication object initialization!";
        return -1;
    }

    return -2;
}
