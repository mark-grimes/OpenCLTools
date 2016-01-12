#include <CL/cl.hpp>

//
// Template specialisation for the CL_DEVICE_SPIR_VERSIONS extension, so that
// I can easily call device.getInfo<CL_DEVICE_SPIR_VERSIONS>().  This uses the
// internals of cl.hpp.
//
#ifdef CL_DEVICE_SPIR_VERSIONS
namespace cl{ namespace detail {
	template<>
	struct param_traits<cl::detail::cl_device_info,CL_DEVICE_SPIR_VERSIONS>
	{
		enum { value = CL_DEVICE_SPIR_VERSIONS };
		typedef cl::STRING_CLASS param_type;
	};
}} // end of namespace cl::detail
#endif // end of "#ifdef CL_DEVICE_SPIR_VERSIONS"

namespace tools
{
	std::string deviceType( cl_device_type type )
	{
		switch( type )
		{
			case CL_DEVICE_TYPE_DEFAULT:
				return "CL_DEVICE_TYPE_DEFAULT";
			case CL_DEVICE_TYPE_CPU:
				return "CL_DEVICE_TYPE_CPU";
			case CL_DEVICE_TYPE_GPU:
				return "CL_DEVICE_TYPE_GPU";
			case CL_DEVICE_TYPE_ACCELERATOR:
				return "CL_DEVICE_TYPE_ACCELERATOR";
			case CL_DEVICE_TYPE_CUSTOM:
				return "CL_DEVICE_TYPE_CUSTOM";
			case CL_DEVICE_TYPE_ALL:
				return "CL_DEVICE_TYPE_ALL";
			default:
				return "<unknown>";
		}
	}

	std::string kernelEnqueError( cl_int error )
	{
        switch( error )
        {
        	case CL_SUCCESS : return "CL_SUCCESS";
        	case CL_INVALID_PROGRAM_EXECUTABLE : return "CL_INVALID_PROGRAM_EXECUTABLE";
        	case CL_INVALID_COMMAND_QUEUE : return "CL_INVALID_COMMAND_QUEUE";
        	case CL_INVALID_KERNEL : return "CL_INVALID_KERNEL";
        	case CL_INVALID_CONTEXT : return "CL_INVALID_CONTEXT";
        	case CL_INVALID_KERNEL_ARGS : return "CL_INVALID_KERNEL_ARGS";
        	case CL_INVALID_WORK_DIMENSION : return "CL_INVALID_WORK_DIMENSION";
        	case CL_INVALID_WORK_GROUP_SIZE : return "CL_INVALID_WORK_GROUP_SIZE";
        	case CL_INVALID_WORK_ITEM_SIZE : return "CL_INVALID_WORK_ITEM_SIZE";
        	case CL_INVALID_GLOBAL_OFFSET : return "CL_INVALID_GLOBAL_OFFSET";
        	case CL_OUT_OF_RESOURCES : return "CL_OUT_OF_RESOURCES";
        	case CL_MEM_OBJECT_ALLOCATION_FAILURE : return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        	case CL_INVALID_EVENT_WAIT_LIST : return "CL_INVALID_EVENT_WAIT_LIST";
        	case CL_OUT_OF_HOST_MEMORY : return "CL_OUT_OF_HOST_MEMORY";
        	default : return "<unknown>";
        }
	}

	std::string contextCreateError( cl_int error )
	{
        switch( error )
        {
        	case CL_SUCCESS : return "CL_SUCCESS";
        	case CL_INVALID_PLATFORM : return "CL_INVALID_PLATFORM";
        	case CL_INVALID_PROPERTY : return "CL_INVALID_PROPERTY";
        	case CL_INVALID_VALUE : return "CL_INVALID_VALUE";
        	case CL_INVALID_DEVICE : return "CL_INVALID_DEVICE";
        	case CL_INVALID_OPERATION : return "CL_INVALID_OPERATION";
        	case CL_DEVICE_NOT_AVAILABLE : return "CL_DEVICE_NOT_AVAILABLE";
        	case CL_OUT_OF_RESOURCES : return "CL_OUT_OF_RESOURCES";
        	case CL_OUT_OF_HOST_MEMORY : return "CL_OUT_OF_HOST_MEMORY";
        	case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR : return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        	default : return "<unknown>";
        }
	}

	std::string createProgramError( cl_int error )
	{
        switch( error )
        {
        	case CL_SUCCESS : return "CL_SUCCESS";
        	case CL_INVALID_CONTEXT : return "CL_INVALID_CONTEXT";
        	case CL_INVALID_VALUE : return "CL_INVALID_VALUE";
        	case CL_OUT_OF_RESOURCES : return "CL_OUT_OF_RESOURCES";
        	case CL_OUT_OF_HOST_MEMORY : return "CL_OUT_OF_HOST_MEMORY";
        	default : return "<unknown>";
        }
	}

	std::string createKernelError( cl_int error )
	{
        switch( error )
        {
        	case CL_SUCCESS : return "CL_SUCCESS";
        	case CL_INVALID_PROGRAM : return "CL_INVALID_PROGRAM";
        	case CL_INVALID_PROGRAM_EXECUTABLE : return "CL_INVALID_PROGRAM_EXECUTABLE";
        	case CL_INVALID_KERNEL_NAME : return "CL_INVALID_KERNEL_NAME";
        	case CL_INVALID_KERNEL_DEFINITION : return "CL_INVALID_KERNEL_DEFINITION";
        	case CL_INVALID_VALUE : return "CL_INVALID_VALUE";
        	case CL_OUT_OF_RESOURCES : return "CL_OUT_OF_RESOURCES";
        	case CL_OUT_OF_HOST_MEMORY : return "CL_OUT_OF_HOST_MEMORY";
        	default : return "<unknown>";
        }
	}

	std::string setKernelArgError( cl_int error )
	{
        switch( error )
        {
        	case CL_SUCCESS : return "CL_SUCCESS";
        	case CL_INVALID_KERNEL : return "CL_INVALID_KERNEL";
        	case CL_INVALID_ARG_INDEX : return "CL_INVALID_ARG_INDEX";
        	case CL_INVALID_ARG_VALUE : return "CL_INVALID_ARG_VALUE";
        	case CL_INVALID_MEM_OBJECT : return "CL_INVALID_MEM_OBJECT";
        	case CL_INVALID_SAMPLER : return "CL_INVALID_SAMPLER";
        	case CL_INVALID_ARG_SIZE : return "CL_INVALID_ARG_SIZE";
        	case CL_OUT_OF_RESOURCES : return "CL_OUT_OF_RESOURCES";
        	case CL_OUT_OF_HOST_MEMORY : return "CL_OUT_OF_HOST_MEMORY";
        	default : return "unknown";
        }
	}

	std::string enqueKernelError( cl_int error )
	{
        switch( error )
        {
        	case CL_SUCCESS : return "CL_SUCCESS";
        	case CL_INVALID_PROGRAM_EXECUTABLE : return "CL_INVALID_PROGRAM_EXECUTABLE";
        	case CL_INVALID_COMMAND_QUEUE : return "CL_INVALID_COMMAND_QUEUE";
        	case CL_INVALID_KERNEL : return "CL_INVALID_KERNEL";
        	case CL_INVALID_CONTEXT : return "CL_INVALID_CONTEXT";
        	case CL_INVALID_KERNEL_ARGS : return "CL_INVALID_KERNEL_ARGS";
        	case CL_INVALID_WORK_DIMENSION : return "CL_INVALID_WORK_DIMENSION";
        	case CL_INVALID_WORK_GROUP_SIZE : return "CL_INVALID_WORK_GROUP_SIZE";
        	case CL_INVALID_WORK_ITEM_SIZE : return "CL_INVALID_WORK_ITEM_SIZE";
        	case CL_INVALID_GLOBAL_OFFSET : return "CL_INVALID_GLOBAL_OFFSET";
        	case CL_OUT_OF_RESOURCES : return "CL_OUT_OF_RESOURCES";
        	case CL_MEM_OBJECT_ALLOCATION_FAILURE : return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        	case CL_INVALID_EVENT_WAIT_LIST : return "CL_INVALID_EVENT_WAIT_LIST";
        	case CL_OUT_OF_HOST_MEMORY : return "CL_OUT_OF_HOST_MEMORY";
        	default : return "<unknown>";
        }
	}

} // end of namespace tools
