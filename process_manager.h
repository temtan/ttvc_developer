// process_manager.h

#pragma once

#include <functional>
#include <vector>
#include <optional>

#include "tt_process.h"
#include "tt_thread.h"

#include "developer.h"
#include "exception.h"


namespace TTVCDeveloper {
  // -- ProcessManager ---------------------------------------------------
  class ProcessManager {
  public:
    using ErrorHandler               = std::function<void ( ProcessCreateException& e )>;
    using OutputReceiver             = std::function<void ( const std::string& data )>;
    using BeforeFunction             = std::function<bool ( void )>;
    using AfterFunction              = std::function<bool ( DWORD exit_code )>;
    using Function                   = std::function<void ( void )>;
    using LastFunction               = std::function<void ( bool exist_error )>;
    using StandardInputStartFunction = std::function<void ( HANDLE handle )>;

    struct Command {
      TtProcess::CreateInfo      info_;
      BeforeFunction             before_;
      AfterFunction              after_;
      StandardInputStartFunction standard_input_start_;
      Function                   standard_input_end_;

      TtProcess               process_;
      std::shared_ptr<TtPipe> output_pipe_;
      std::shared_ptr<TtPipe> input_pipe_;
      bool use_standard_input_;
    };

    explicit ProcessManager( void );

    void SetErrorHandler( ErrorHandler error_handler );
    void SetOutputReceiver( OutputReceiver receiver );
    void SetInitializer( Function initializer );
    void SetFinalizer( Function finalizer );
    void SetFirst( BeforeFunction function );
    void SetLast( LastFunction function );

    std::vector<Command>& GetCommands( void );

    void ThreadStart( void );

  private:
    void CreateProcess( Command& command );
    void WaitProcess( Command& command );

  private:
    ErrorHandler   error_handler_;
    OutputReceiver receiver_;
    Function       initializer_;
    Function       finalizer_;
    BeforeFunction first_;
    LastFunction   last_;

    std::vector<Command> commands_;

    std::optional<TtFunctionThread> thread_;
  };
}
