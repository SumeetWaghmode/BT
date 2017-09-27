package com.accenture.rac.table;

import org.eclipse.swt.graphics.Image;

import com.accenture.rac.Activator;

public enum Images {
	
	/** The checked. */
	CHECKED(Activator.getImageDescriptor("icons/checked.gif").createImage()),
	
	/** The unchecked. */
	UNCHECKED(Activator.getImageDescriptor("icons/unchecked.gif").createImage());
	
	/** The image. */
	private Image image;
	
	/**
	 * Instantiates a new images.
	 *
	 * @param image the image
	 */
	private Images(Image image){
        this.image = image;
    }
	
	/**
	 * Gets the image.
	 *
	 * @return the image
	 */
	public Image getImage() {
		return image;
	}
}
