/*---------------------------------------------------------------------------*
*  Copyright (c) 2010        Jefferson Science Associates,                   *
*                            Thomas Jefferson National Accelerator Facility  *
*                                                                            *
*    This software was developed under a United States Government license    *
*    described in the NOTICE file included as part of this distribution.     *
*                                                                            *
*    C.Timmer, 22-Nov-2010, Jefferson Lab                                    *
*                                                                            *
*    Authors: Carl Timmer                                                    *
*             timmer@jlab.org                   Jefferson Lab, #10           *
*             Phone: (757) 269-5130             12000 Jefferson Ave.         *
*             Fax:   (757) 269-6248             Newport News, VA 23606       *
*                                                                            *
*----------------------------------------------------------------------------*/

package org.jlab.coda.cMsg.remoteExec;

import org.jlab.coda.cMsg.cMsgException;

/**
 * An object of this class is returned by calls to the
 * {@link Commander#startProcess} or {@link Commander#startThread}
 * methods and is used to track the process of command given
 * to an Executor.
 *
 * @author timmer
 * Date: Oct 14, 2010
 */
public class CommandReturn {

    /** Id number generated by Commander. */
    private int id;

    /** Associated id number generated by Executor. */
    private int executorId;

    /** Output (if any) of error. */
    private String errorOutput;

    /** Output (if any) of the started process. */
    private String regularOutput;

    /** Was there an error in running the process/thread? */
    private boolean error;

    /** Has the process already terminated? */
    private boolean terminated;

    /** Callback object. Sorry, only one allowed. */
    private CommandCallback processCallback;

    /** Object to pass to callback as argument. */
    private Object userObject;

    /** What is the state of the callback? */
    private CallbackState callbackState;

    /**
     * When this is true, no more changes should be made to this object.
     * This is to prevent a possible race condition in which the Commander's
     * main callback receives an "I'm done" msg from the Executor <b>BEFORE</b>
     * the return of the startProcess/Thread methods' sendAndGet which then sets
     * members in this object to old values.
     */
    private boolean locked;

    /** The Commander which created this object. */
    private Commander commander;

    /** The Executor sending the results defining this object. */
    private ExecutorInfo executor;


    /**
     * Constructor.
     * @param commander Commander object which is calling this method.
     * @param executor ExecutorInfo object the Commander is talking to.
     * @param callbackState state of the callback
     */
    CommandReturn(Commander commander, ExecutorInfo executor, CallbackState callbackState) {
        this.executor = executor;
        this.commander = commander;
        this.callbackState = callbackState;
    }


    /**
     * Constructor.
     *
     * @param commander Commander object which is calling this method.
     * @param executor ExecutorInfo object the Commander is talking to.
     * @param id id number generated by Commander.
     * @param processId associated id number generated by Executor.
     * @param error was there an error running the process/thread?
     * @param terminated has process terminated already?
     * @param regularOutput regular output (if any) of the started process
     * @param errorOutput error output (if any) of the started process
     */
    CommandReturn(Commander commander, ExecutorInfo executor,
                  int id, int processId, boolean error, boolean terminated,
                  String regularOutput, String errorOutput) {

        this.executor = executor;
        this.commander = commander;
        setValues(id, processId, error, terminated, regularOutput, errorOutput);
    }


    /**
     * Set various member values, but not when locked.
     *
     * @param id id number generated by Commander.
     * @param processId associated id number generated by Executor.
     * @param error was there an error running the process/thread?
     * @param terminated has process terminated already?
     * @param regularOutput regular output (if any) of the started process
     * @param errorOutput error output (if any) of the started process
     */
    synchronized public void setValues(int id, int processId, boolean error, boolean terminated,
                          String regularOutput, String errorOutput) {
        if (locked) return;

        // ensure consistency
        if (errorOutput != null) error = true;

        this.id = id;
        this.executorId = processId;
        this.error = error;
        this.terminated = terminated;
        this.regularOutput = regularOutput;
        this.errorOutput = errorOutput;
    }

    /**
     * Stop the process or thread on the executor this object is associated with.
     */
    public void stop() {
        try {
            commander.stop(executor, id);
        }
        catch (cMsgException e) { }
    }

    /**
     * Is this object locked from further updates?
     * @return <code>true</code> if locked, else <code>false</code>
     */
    synchronized public boolean isLocked() {
        return locked;
    }

    /**
     * Lock this object from further updates.
     */
    synchronized void lock() {
        locked = true;
    }

    /**
     * Register a callback to be run when the process ends.
     * Calling this multiple times results in a replacement of the callback.
     *
     * @param processCallback callback to be run when the process ends.
     * @param userObject argument to be passed to callback.
     */
    synchronized public void registerCallback(CommandCallback processCallback, Object userObject) {
        this.userObject = userObject;
        this.processCallback = processCallback;
    }

    /**
     * Unregister a callback so it does not get run.
     */
    synchronized public void unregisterCallback() {
        this.userObject = null;
        this.processCallback = null;
        if (callbackState == CallbackState.PENDING) {
            callbackState = CallbackState.CANCELLED;
        }
    }

    /**
     * Run the registered callback.
     */
    synchronized public void executeCallback() {
        if (processCallback == null) return;
        processCallback.callback(userObject, this);
        callbackState = CallbackState.RUN;
    }

    /**
     * Has the callback been cancelled for any reason?
     * @return <code>true</code> if callback has already been cancelled, else <code>false</code>.
     */
    synchronized public boolean callbackCancelled() {
        return (callbackState != CallbackState.RUN &&
                callbackState != CallbackState.NONE &&
                callbackState != CallbackState.PENDING);
    }

    /**
     * Get the state of the callback.
     * @return the state of the callback.
     */
    synchronized public CallbackState getCallbackState() {
        return callbackState;
    }

    /**
     * Set the state of the callback, but not when locked.
     * @param callbackState state of the callback.
     */
    synchronized void setCallbackState(CallbackState callbackState) {
        if (locked) return;
        this.callbackState = callbackState;
    }

    /**
     * Set the state of the callback even if locked.
     * @param callbackState state of the callback.
     */
    synchronized void setCallbackStateIfLocked(CallbackState callbackState) {
        this.callbackState = callbackState;
    }

    /**
     * Get the Executor generated id number.
     * @return the Executor generated id number.
     */
    synchronized int getExecutorId() {
        return executorId;
    }

    /**
     * Get the Commander generated id number.
     * @return the Commander generated id number.
     */
    synchronized public int getId() {
        return id;
    }

    /**
     * Set whether the process has already terminated.
     * @param b <code>true</code> if process has terminated, else <code>false</code>.
     */
    synchronized void hasTerminated(boolean b) {
        terminated = b;
    }

    /**
     * Get whether the process has already terminated.
     * @return <code>true</code> if process has already terminated, else <code>false</code>.
     */
    synchronized public boolean hasTerminated() {
        return terminated;
    }

    /**
     * Get whether error occurred in the attempt to run the process or thread.
     * @return <code>true</code> if error occurred attempting to run process/thread,
     *         else <code>false</code>.
     */
    synchronized public boolean hasError() {
        return error;
    }

    /**
     * Get whether the process has any output.
     * @return <code>true</code> if the process has output, else <code>false</code>.
     */
    synchronized public boolean hasOutput() {
        return regularOutput != null;
    }

    /**
     * Get output (if any) generated by the started process or thread.
     * @return any output by the process or thread, or null if none
     */
    synchronized public String getOutput() {
        return regularOutput;
    }

    /**
     * Get error output (if any) generated by the started process or thread.
     * @return any error output by the process or thread, or null if none
     */
    synchronized public String getError() {
        return errorOutput;
    }

    /**
     * Set error output generated by the started process or thread.
     * @param error error output by the process or thread.
     */
    synchronized void setError(String error) {
        errorOutput = error;
        if (error != null) this.error = true;
    }

    /**
     * Set output generated by the started process or thread.
     * @param output output by the process or thread.
     */
    synchronized void setOutput(String output) {
        regularOutput = output;
    }

    public String toString() {
        return "Id = " + id + ", execId = " + executorId + ", error = " +
                error + ", term = " + terminated + ", output = " + (regularOutput != null)  +
                ", error output = " + (errorOutput != null);
    }

}