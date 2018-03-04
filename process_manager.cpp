// process_manager.cpp


#include "process_manager.h"

using namespace TTVCDeveloper;

// -- ProcessManager -----------------------------------------------------
ProcessManager::ProcessManager( void )
{
}


void
ProcessManager::SetErrorHandler( ErrorHandler error_handler )
{
  error_handler_ = error_handler;
}

void
ProcessManager::SetOutputReceiver( OutputReceiver receiver )
{
  receiver_ = receiver;
}

void
ProcessManager::SetInitializer( Function initializer )
{
  initializer_ = initializer;
}

void
ProcessManager::SetFinalizer( Function finalizer )
{
  finalizer_ = finalizer;
}

void
ProcessManager::SetFirst( BeforeFunction function )
{
  first_ = function;
}

void
ProcessManager::SetLast( LastFunction function )
{
  last_ = function;
}


std::vector<ProcessManager::Command>&
ProcessManager::GetCommands( void )
{
  return commands_;
}


void
ProcessManager::ThreadStart( void )
{
  std::function<void ( void )> tmp = [&] ( void ) {
    TtUtility::Safing( initializer_ )();

    TtUtility::DestructorCall destructor_call( [&] ( void ) {
      TtUtility::Safing( finalizer_ )();
    } );

    try {
      bool exist_error = NOT( TtUtility::Safing( first_, true )() );

      if ( NOT( exist_error ) ) {
        for ( auto& command : commands_ ) {
          if ( NOT( TtUtility::Safing( command.before_, true )() ) ) {
            exist_error = true;
            break;
          }

          this->CreateProcess( command );
          this->WaitProcess( command );

          if ( NOT( TtUtility::Safing( command.after_, command.process_.GetExitCode() == 0 )( command.process_.GetExitCode() ) ) ) {
            exist_error = true;
            break;
          }
        }
      }

      TtUtility::Safing( last_ )( exist_error );
    }
    catch ( ProcessCreateException& e ) {
      TtUtility::Safing( error_handler_ )( e );
    }
  };

  thread_.emplace( tmp );
  thread_->Start();
}


void
ProcessManager::CreateProcess( Command& command )
{
PCD();
  TtProcess::CreateInfo tmp_info = command.info_;
  tmp_info.SetInheritHandles( true );

  command.output_pipe_ = std::make_shared<TtPipe>();
  command.output_pipe_->GetReadPipeHandle().SetInherit( false );
  tmp_info.SetStandardOutput( command.output_pipe_->GetWritePipeHandle() );

  TtPipe::Handle for_error = command.output_pipe_->GetWritePipeHandle().Duplicate( true );
  tmp_info.SetStandardError( for_error );

PCD();
  if ( command.use_standard_input_ ) {
PCD();
    command.input_pipe_ = std::make_shared<TtPipe>();
    command.input_pipe_->GetWritePipeHandle().SetInherit( false );
    tmp_info.SetStandardInput( command.input_pipe_->GetReadPipeHandle() );
  }

  command.process_ = TtProcess();
  try {
    command.process_.Create( tmp_info );
  }
  catch ( TtWindowsSystemCallException& e ) {
    throw ProcessCreateException( tmp_info, e.GetErrorNumber() );
  }

  command.output_pipe_->GetWritePipeHandle().Close();
  for_error.Close();
PCD();
  if ( command.use_standard_input_ ) {
PCD();
    command.input_pipe_->GetReadPipeHandle().Close();
    TtUtility::Safing( command.standard_input_start_ )( command.input_pipe_->GetWritePipeHandle().GetWindowsHandle() );
  }
}

void
ProcessManager::WaitProcess( Command& command )
{
  TtString::UniqueString buffer( 1024 );
  for (;;) {
    DWORD received;
    if ( ::ReadFile( command.output_pipe_->GetReadPipeHandle().GetWindowsHandle(),
                     buffer.GetPointer(), buffer.GetCapacity() - 1, &received, NULL ) == 0 ) {
      break;
    }
    buffer.GetPointer()[received] = '\0';
    TtUtility::Safing( receiver_ )( buffer.GetPointer() );
  }
  command.process_.Wait();
PCD();
  if ( command.use_standard_input_ ) {
PCD();
    TtUtility::Safing( command.standard_input_end_ )();
    command.input_pipe_->GetWritePipeHandle().Close();
  }
  command.output_pipe_->GetReadPipeHandle().Close();
}
