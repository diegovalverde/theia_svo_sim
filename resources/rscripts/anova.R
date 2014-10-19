
data = read.table("experiment.results.log", header = TRUE, sep = ",")

fit = aov(mem_external_read_access_count ~ 
      octree_depth * 
      gpu_grid_partition_size *
      gpu_memory_cache_enabled * 
      gpu_memory_cache_lines_per_way, data=data)

layout(matrix(c(1,2,3,4),2,2)) 
summary(fit)

interaction.plot(data$octree_depth, data$gpu_grid_partition_size, data$mem_external_read_access_count)
interaction.plot(data$octree_depth, data$gpu_memory_cache_enabled, data$mem_external_read_access_count)
interaction.plot(data$octree_depth, data$gpu_memory_cache_lines_per_way, data$mem_external_read_access_count)
