/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.gephi.libgexf;

public class RngValidator {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected RngValidator(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(RngValidator obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libgexfJNI.delete_RngValidator(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public static boolean run(String xml_file_pathname, String rng_file_pathname) {
    return libgexfJNI.RngValidator_run(xml_file_pathname, rng_file_pathname);
  }

}