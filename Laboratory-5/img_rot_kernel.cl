__kernel void img_rot(
  __global float *in,
  __global float *out,
    const float angle){

  int i = get_global_id(0); //as in slides, assuming a 1 dimension worksize

  if(i < count){
    out[i] = in[i] * in[i];
  }
}