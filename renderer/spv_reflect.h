#ifndef SPV_REFLECT_H
#define SPV_REFLECT_H


//Todo: create an out struct for all the data i would want
typedef struct spirv_refect_info
{
    int descriptor_set_count;
    int descriptor_binding_count;
    int descriptor_set_types;
    //etc...
} spirv_refect_info;

typedef struct spirv_reflect_descriptor_set_info
{
    SpvReflectShaderStageFlagBits* stage_flags;
    u32 descriptor_set_count;
    u32* binding_number;
    SpvReflectDescriptorType* descriptor_set_types;
} spirv_reflect_descriptor_set_info;

typedef struct spirv_reflect_input_variable_info
{
    u32 input_count;
    u32* locations;
    SpvReflectFormat* formats;
    u32* offsets;
} spirv_reflect_input_variable_info;

typedef struct
{
    uint32_t flag;
    const char* name;
} ShaderStageName;

static const ShaderStageName ShaderStageTable[] = {
    {SPV_REFLECT_SHADER_STAGE_VERTEX_BIT, "VERTEX"},
    {SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "TESSELLATION_CONTROL"},
    {SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "TESSELLATION_EVALUATION"},
    {SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT, "GEOMETRY"},
    {SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT, "FRAGMENT"},
    {SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT, "COMPUTE"},
    {SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV, "TASK_NV"},
    {SPV_REFLECT_SHADER_STAGE_TASK_BIT_EXT, "TASK_EXT"},
    {SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV, "MESH_NV"},
    {SPV_REFLECT_SHADER_STAGE_MESH_BIT_EXT, "MESH_EXT"},
    {SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR, "RAYGEN_KHR"},
    {SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR, "ANY_HIT_KHR"},
    {SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, "CLOSEST_HIT_KHR"},
    {SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR, "MISS_KHR"},
    {SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR, "INTERSECTION_KHR"},
    {SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR, "CALLABLE_KHR"}
};


const char* SpvReflectDescriptorType_TABLE[] = {
    "SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC",
    "SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT",
    "SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR",
};


static const int ShaderStageTableCount = sizeof(ShaderStageTable) / sizeof(ShaderStageTable[0]);


spirv_refect_info* spriv_reflection_testing(const char* shader_path)
{
    //TODO: add a type, vert, frag, (or frag and vert combined) compute etc

    spirv_refect_info* out_reflect_info = malloc(sizeof(spirv_refect_info));

    file_read_data shader_data = {0};


    filesystem_open_and_return_bytes("../renderer/shaders/spirv_reflect_test.vert.spv", &shader_data);


    SpvReflectShaderModule module = {0};
    SpvReflectResult result = spvReflectCreateShaderModule(shader_data.size, shader_data.data, &module);
    MASSERT_MSG(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule failed");
    TRACE("SpvReflectCreateShaderModule result SUCCESS")

    // this has an additional flag param i dont need rn
    // SpvReflectResult result2 = spvReflectCreateShaderModule2();
    // SpvReflectModuleFlags a;
    // Enumerate and extract shader's input variables


    uint32_t input_var_count = 0;
    spvReflectEnumerateInputVariables(&module, &input_var_count, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectInterfaceVariable** inputs_variables =
            (SpvReflectInterfaceVariable **) malloc(input_var_count * sizeof(SpvReflectInterfaceVariable *));
    spvReflectEnumerateInputVariables(&module, &input_var_count, inputs_variables);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    for (u32 i = 0; i < input_var_count; i++)
    {
        //format of the in values
        TRACE("FORMAT %d", inputs_variables[i]->format);
        TRACE("spirv_id %d", inputs_variables[i]->spirv_id);
        TRACE("stride %d", inputs_variables[i]->array.stride);
    }


    uint32_t d_set_count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &d_set_count, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectDescriptorSet** d_sets =
            (SpvReflectDescriptorSet **) malloc(d_set_count * sizeof(SpvReflectDescriptorSet *));
    result = spvReflectEnumerateDescriptorSets(&module, &d_set_count, d_sets);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);


    DEBUG("D-Set COUNTsss %d", d_set_count);
    for (u32 i = 0; i < d_set_count; i++)
    {
        //format of the in values
        TRACE("D-Set set %d", d_sets[i]->set);
        TRACE("D-Set BINDING COUNT %d", d_sets[i]->binding_count);
        for (u32 j = 0; j < d_sets[i]->binding_count; j++)
        {
            TRACE("BINDING NAME: %s", d_sets[i]->bindings[j]->name);
            DEBUG("BINDING NUMBER %d", d_sets[i]->bindings[j]->binding);
            INFO("type %d, NAME: %s", d_sets[i]->bindings[j]->descriptor_type,
                 SpvReflectDescriptorType_TABLE[d_sets[i]->bindings[j]->descriptor_type]);
        }
    }


    //NOTE: kinda no need for this since this data is contained in d_sets
    uint32_t d_binding_count = 0;
    spvReflectEnumerateDescriptorBindings(&module, &d_binding_count, NULL);
    SpvReflectDescriptorBinding** d_bindings =
            (SpvReflectDescriptorBinding **) malloc(d_binding_count * sizeof(SpvReflectDescriptorBinding *));
    spvReflectEnumerateDescriptorBindings(&module, &d_binding_count, d_bindings);


    uint32_t output_var_count = 0;
    spvReflectEnumerateOutputVariables(&module, &output_var_count, NULL);
    SpvReflectInterfaceVariable** ouput_variables =
            (SpvReflectInterfaceVariable **) malloc(output_var_count * sizeof(SpvReflectInterfaceVariable *));
    spvReflectEnumerateOutputVariables(&module, &output_var_count, ouput_variables);

    uint32_t push_constant_count = 0;
    spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, NULL);
    SpvReflectBlockVariable** push_constants =
            (SpvReflectBlockVariable **) malloc(push_constant_count * sizeof(SpvReflectBlockVariable *));
    spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, push_constants);

    uint32_t specialized_constant_count = 0;
    spvReflectEnumerateSpecializationConstants(&module, &specialized_constant_count, NULL);
    SpvReflectSpecializationConstant** specialized_constants =
            (SpvReflectSpecializationConstant **) malloc(
                specialized_constant_count * sizeof(SpvReflectSpecializationConstant *));
    spvReflectEnumerateSpecializationConstants(&module, &specialized_constant_count, specialized_constants);

    // free(d_sets);
    // free(d_bindings);
    // free(specialized_constants);
    // free(push_constants);
    // free(ouput_variables);
    // free(inputs_variables);

    spvReflectDestroyShaderModule(&module);


    return out_reflect_info;
}


const char* GetShaderStageName(uint32_t flag)
{
    for (int i = 0; i < ShaderStageTableCount; ++i)
    {
        if (ShaderStageTable[i].flag == flag)
            return ShaderStageTable[i].name;
    }
    return "UNKNOWN_STAGE";
}

spirv_reflect_descriptor_set_info* spriv_reflect_get_descriptor_set(Arena* frame_arena,
                                                                    const char* vertex_shader_path,
                                                                    const char* fragment_shader_path)
{
    //TODO: arena
    //TODO: append the .vert.spv, and .frag.spv so i only have to pass in the path/name
    spirv_reflect_descriptor_set_info* out_reflect_info = malloc(sizeof(spirv_reflect_descriptor_set_info));

    file_read_data vert_shader_data = {0};
    file_read_data frag_shader_data = {0};

    filesystem_open_and_return_bytes(vertex_shader_path, &vert_shader_data);
    filesystem_open_and_return_bytes(fragment_shader_path, &frag_shader_data);


    SpvReflectShaderModule vert_module = {0};
    SpvReflectResult result = spvReflectCreateShaderModule(vert_shader_data.size, vert_shader_data.data, &vert_module);
    MASSERT_MSG(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule VERT failed");
    TRACE("SpvReflectCreateShaderModule VERT result SUCCESS")

    SpvReflectShaderModule frag_module = {0};
    result = spvReflectCreateShaderModule(frag_shader_data.size, frag_shader_data.data, &frag_module);
    MASSERT_MSG(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule FRAG failed");
    TRACE("SpvReflectCreateShaderModule FRAG result SUCCESS")

    INFO("STAGE TYPE %p, %s", vert_module.shader_stage, GetShaderStageName(vert_module.shader_stage));
    INFO("STAGE TYPE %p, %s", frag_module.shader_stage, GetShaderStageName(frag_module.shader_stage));


    //vert
    uint32_t d_set_count_vert = 0;
    result = spvReflectEnumerateDescriptorSets(&vert_module, &d_set_count_vert, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectDescriptorSet** d_sets_vert =
            (SpvReflectDescriptorSet **) malloc(d_set_count_vert * sizeof(SpvReflectDescriptorSet *));
    result = spvReflectEnumerateDescriptorSets(&vert_module, &d_set_count_vert, d_sets_vert);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

    //frag
    uint32_t d_set_count_frag = 0;
    result = spvReflectEnumerateDescriptorSets(&frag_module, &d_set_count_frag, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectDescriptorSet** d_sets_frag =
            (SpvReflectDescriptorSet **) malloc(d_set_count_frag * sizeof(SpvReflectDescriptorSet *));
    result = spvReflectEnumerateDescriptorSets(&frag_module, &d_set_count_frag, d_sets_frag);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

    out_reflect_info->descriptor_set_count = d_set_count_vert + d_set_count_frag;
    out_reflect_info->stage_flags = darray_create_reserve(SpvReflectShaderStageFlagBits,
                                                          out_reflect_info->descriptor_set_count);
    out_reflect_info->binding_number = darray_create_reserve(u32, out_reflect_info->descriptor_set_count);
    out_reflect_info->descriptor_set_types = darray_create_reserve(SpvReflectDescriptorType,
                                                                   out_reflect_info->descriptor_set_count);

    //DEBUG
    TRACE("D-Set COUNT VERT %d", d_set_count_vert);
    for (u32 i = 0; i < d_set_count_vert; i++)
    {
        darray_push(out_reflect_info->stage_flags, vert_module.shader_stage);

        //format of the in values
        TRACE("D-Set BINDING COUNT %d", d_sets_vert[i]->binding_count);
        for (u32 j = 0; j < d_sets_vert[i]->binding_count; j++)
        {
            darray_push(out_reflect_info->binding_number, d_sets_vert[i]->bindings[j]->binding);
            darray_push(out_reflect_info->descriptor_set_types, d_sets_vert[i]->bindings[j]->descriptor_type);

            TRACE("BINDING NAME: %s", d_sets_vert[i]->bindings[j]->name);
            DEBUG("BINDING NUMBER %d", d_sets_vert[i]->bindings[j]->binding);
            DEBUG("SET BINDING NUMBER %d", d_sets_vert[i]->bindings[j]->set);

            INFO("type %d, NAME: %s", d_sets_vert[i]->bindings[j]->descriptor_type,
                 SpvReflectDescriptorType_TABLE[d_sets_vert[i]->bindings[j]->descriptor_type]);
        }
        TRACE("D-Set set %d", d_sets_vert[i]->set);
    }

    TRACE("D-Set COUNT FRAG %d", d_set_count_frag);
    for (u32 i = 0; i < d_set_count_frag; i++)
    {
        darray_push(out_reflect_info->stage_flags, frag_module.shader_stage);

        TRACE("D-Set FRAG BINDING COUNT %d", d_sets_frag[i]->binding_count);
        for (u32 j = 0; j < d_sets_frag[i]->binding_count; j++)
        {
            darray_push(out_reflect_info->binding_number, d_sets_frag[i]->bindings[j]->binding);
            darray_push(out_reflect_info->descriptor_set_types, d_sets_frag[i]->bindings[j]->descriptor_type);


            TRACE("BINDING NAME FRAG: %s", d_sets_frag[i]->bindings[j]->name);
            DEBUG("BINDING NUMBER %d", d_sets_frag[i]->bindings[j]->binding);
            DEBUG("SET BINDING NUMBER %d", d_sets_frag[i]->bindings[j]->set);
            INFO("type %d, NAME FRAG: %s", d_sets_frag[i]->bindings[j]->descriptor_type,
                 SpvReflectDescriptorType_TABLE[d_sets_frag[i]->bindings[j]->descriptor_type]);
        }
        TRACE("D-Set FRAG set %d", d_sets_frag[i]->set);
    }


    // free(d_sets_vert);
    // free(d_sets_frag);

    spvReflectDestroyShaderModule(&vert_module);


    return out_reflect_info;
}



spirv_reflect_input_variable_info* spriv_reflect_get_input_variable(Arena* frame_arena, const char* shader_path)
{
    //TODO: arena
    spirv_reflect_input_variable_info* out_reflect_info = malloc(sizeof(spirv_reflect_descriptor_set_info));

    file_read_data vert_shader_data = {0};

    //TODO: SHADER PATH
    filesystem_open_and_return_bytes(shader_path, &vert_shader_data);


    SpvReflectShaderModule vert_module = {0};
    SpvReflectResult result = spvReflectCreateShaderModule(vert_shader_data.size, vert_shader_data.data, &vert_module);
    MASSERT_MSG(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule VERT failed");
    TRACE("SpvReflectCreateShaderModule VERT result SUCCESS")

    INFO("STAGE TYPE %p, %s", vert_module.shader_stage, GetShaderStageName(vert_module.shader_stage));




    uint32_t input_count = 0;
    spvReflectEnumerateInputVariables(&vert_module, &input_count, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectInterfaceVariable** vert_input_variables =
            (SpvReflectInterfaceVariable **) malloc(input_count * sizeof(SpvReflectInterfaceVariable *));
    spvReflectEnumerateInputVariables(&vert_module, &input_count, vert_input_variables);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);


    out_reflect_info->input_count = input_count;
    out_reflect_info->locations = darray_create_reserve(u32, input_count);
    out_reflect_info->formats = darray_create_reserve(SpvReflectFormat, input_count);
    out_reflect_info->offsets = darray_create_reserve(u32, input_count+1); // additional index value never gets used
    darray_push(out_reflect_info->offsets, 0); //we always start at 0

    u32 final_input_count = input_count;
    for (u32 i = 0; i < input_count; i++)
    {
        DEBUG("INPUT NAME %s", vert_input_variables[i]->name);
        DEBUG("LOCATION %d", vert_input_variables[i]->location);
        DEBUG("FORMAT %d", vert_input_variables[i]->format);
        DEBUG("OFFSET VECTOR COUNT %d ", vert_input_variables[i]->numeric.vector.component_count);
        DEBUG("OFFSET MAT ROW/COL %d %d", vert_input_variables[i]->numeric.matrix.row_count,
              vert_input_variables[i]->numeric.matrix.column_count);

        //build in types have invalid locations, so we check for those and skip them
        if (vert_input_variables[i]->location == UINT32_MAX)
        {
            final_input_count--;
            continue;
        }

        darray_push(out_reflect_info->locations, vert_input_variables[i]->location);
        darray_push(out_reflect_info->formats, vert_input_variables[i]->format);
        //determine if it's the offset of the matrix or just a vec
        if (vert_input_variables[i]->numeric.matrix.column_count > 0)
        {
            u32 mat_col = vert_input_variables[i]->numeric.matrix.column_count;
            u32 mat_row = vert_input_variables[i]->numeric.matrix.row_count;
            darray_push(out_reflect_info->offsets, mat_col * mat_row * sizeof(f32));
        }
        else
        {
            darray_push(out_reflect_info->offsets,
                        vert_input_variables[i]->numeric.vector.component_count * sizeof(f32));
        }
    }

    // free(d_sets_vert);

    //update to get the proper amount of valid input counts
    out_reflect_info->input_count = final_input_count;


    spvReflectDestroyShaderModule(&vert_module);


    return out_reflect_info;
}


#endif //SPV_REFLECT_H
