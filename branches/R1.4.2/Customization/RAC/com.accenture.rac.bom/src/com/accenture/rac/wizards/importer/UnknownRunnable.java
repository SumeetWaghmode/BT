package com.accenture.rac.wizards.importer;

import java.util.concurrent.atomic.AtomicBoolean;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.operation.IRunnableWithProgress;

/**
 * This class represents a long running operation,
 * it will run until the loading flag is set to false
 */
public class UnknownRunnable implements IRunnableWithProgress {
	// The total sleep time
	private volatile AtomicBoolean running;
	

	private String message = "Processing..";
	
	public UnknownRunnable(AtomicBoolean loading) {
		this.running = loading;
	}

	/**
	 * @return the message
	 */
	public String getMessage() {
		return message;
	}

	/**
	 * @param message the message to set
	 */
	public void setMessage(String message) {
		this.message = message;
	}

	/**
	 * Runs the long running operation
	 * 
	 * @param monitor
	 *            the progress monitor
	 */
	public void run(IProgressMonitor monitor) throws InterruptedException {
		
		monitor.beginTask(this.message, IProgressMonitor.UNKNOWN );
		while(running.get()) {
			Thread.sleep(50);
			monitor.worked(500);
		}
		monitor.done();
	}
}