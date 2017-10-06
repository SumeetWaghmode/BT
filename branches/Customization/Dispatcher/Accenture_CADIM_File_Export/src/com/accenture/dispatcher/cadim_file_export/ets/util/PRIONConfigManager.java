/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.accenture.dispatcher.cadim_file_export.ets.util;

//import com.sun.org.apache.xerces.internal.dom.DocumentImpl;
//import com.sun.org.apache.xml.internal.serialize.OutputFormat;
//import com.sun.org.apache.xml.internal.serialize.XMLSerializer;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.apache.xerces.dom.DocumentImpl;
import org.apache.xml.serialize.OutputFormat;
import org.apache.xml.serialize.XMLSerializer;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import com.teamcenter.ets.request.TranslationRequest;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.exceptions.NotLoadedException;
import com.teamcenter.tstk.util.log.ITaskLogger;

/**
 * 
 * @author ftorres
 */
public class PRIONConfigManager {

	private static ITaskLogger serviceLogger = null;
	//private static String serviceHome = "";
	//private static String translatorsConfigFile = ""; // moduleHome +
														// "/conf/translator.xml";
	private static Document dom = null;
	private static Element prionOptionsNode = null;
	private static String TRANSLATED_OBJECT_NAME = null;
	private static String SELECTED_OBJECT_NAME = null;
	private static String FILE_UID = null;
	private static String CADIM_ENTRY_UID = null;

	// private static String translatorsConfigFile =
	// System.getProperty("Translator.PRION.priontranslator.Transmodule.Conf");
	@SuppressWarnings({ "rawtypes", "unchecked" })
	static public void RetrieveTransaltorConfig(String serviceName,
			Vector PRIONDSPDatasetBean_vector, boolean dispatcherClient,
			ITaskLogger myLogger) {

		PRIONDSPDatasetBean priondspDatasetBean = new PRIONDSPDatasetBean();
		priondspDatasetBean.setDataSetType("NONE");
		PRIONDSPDatasetBean_vector.add(priondspDatasetBean);

		// serviceLogger = myLogger;
		// Properties prop = System.getProperties();
		// Enumeration propEnum = prop.propertyNames();
		// serviceLogger.debug("PRIONConfigManager -> System PROPERTIES");
		// while (propEnum.hasMoreElements()) {
		// serviceLogger.debug("      -> Prop: "
		// + (String) propEnum.nextElement());
		// }
		// serviceLogger.debug("      -> dispactherclient.home: ");
		// serviceLogger.debug("         -> "
		// + System.getProperty("dispatcherclient.home"));
		//
		// if (dispatcherClient) {
		// serviceHome = (System.getProperty("dispatcherclient.home"));
		//
		// prop = new Properties();
		// File f = new File(serviceHome + "\\conf\\Service.properties");
		// if (f.exists()) {
		// FileInputStream in = null;
		// try {
		// in = new FileInputStream(f);
		// prop.load(in);
		// // rest of code that handle the values from file....
		// } catch (IOException ex) {
		// Logger.getLogger(PRIONConfigManager.class.getName()).log(
		// Level.SEVERE, null, ex);
		// } finally {
		// try {
		// in.close();
		// } catch (IOException ex) {
		// Logger.getLogger(PRIONConfigManager.class.getName())
		// .log(Level.SEVERE, null, ex);
		// }
		// }
		// } else {
		// serviceLogger
		// .debug("         -> Properties file does not exist!!!");
		//
		// serviceLogger.debug("         ->  " + serviceHome
		// + "\\conf\\Service.properties");
		// }
		// translatorsConfigFile = prop
		// .getProperty("Translator.Accenture.cadim_file_export.moduletranslatorxml.Path");
		// } else {
		// serviceHome = System.getProperty("module.home");
		// translatorsConfigFile = serviceHome + "\\conf\\translator.xml";
		// }
		//
		// serviceLogger.debug("      translatorsConfigFile-> "
		// + translatorsConfigFile);
		//
		// parseXmlFile(translatorsConfigFile);
		// parseDocument(serviceName, PRIONDSPDatasetBean_vector);

	}

	@SuppressWarnings("unused")
	static private void parseXmlFile(String fileLocation) {
		// get the factory
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			// Using factory get an instance of document builder
			DocumentBuilder db = dbf.newDocumentBuilder();
			// parse using builder to get DOM representation of the XML file
			dom = db.parse(fileLocation);
		} catch (ParserConfigurationException pce) {
			pce.printStackTrace();
		} catch (SAXException se) {
			se.printStackTrace();
		} catch (IOException ioe) {
			ioe.printStackTrace();
		}
	}

	@SuppressWarnings({ "unchecked", "rawtypes", "unused" })
	static private void parseDocument(String serviceName,
			Vector PRIONDSPDatasetBean_vector) {
		// get the root element
		Element docEle = dom.getDocumentElement();

		// get a nodelist of elements
		NodeList nl = docEle.getChildNodes();

		if (nl != null && nl.getLength() > 0) {
			for (int x = 0; x < nl.getLength(); x++) {
				if (nl.item(x).getNodeType() == Node.ELEMENT_NODE) {
					if (isPrionTranslator(serviceName, nl.item(x))) {
						PRIONDSPDatasetBean dsBean = new PRIONDSPDatasetBean();
						NodeList prnl = prionOptionsNode.getChildNodes();
						for (int y = 0; y < prnl.getLength(); y++) {
							serviceLogger.debug("PRIONConfigManager -> Node: "
									+ y + " Type: "
									+ prnl.item(y).getNodeType());
							if (prnl.item(y).getNodeType() == Node.ELEMENT_NODE) {
								serviceLogger.debug("      -> Node: " + y
										+ " I AM AN ELEMENT");
								Element pre = (Element) prnl.item(y);
								serviceLogger.debug("      -> My Name is: "
										+ pre.getNodeName());
								if (pre.getNodeName().toLowerCase()
										.equals("dataset")) {
									getDataset(pre, dsBean);
									PRIONDSPDatasetBean_vector
											.add(new PRIONDSPDatasetBean(dsBean));
									dsBean.clean();
								}
							}
						}
					}
				}
			}
		}
	}

	static private void getDataset(Element e, PRIONDSPDatasetBean dsBean) {
		serviceLogger.debug("PRIONConfigManager -> getDataset()");
		serviceLogger.debug("        -> type: " + e.getAttribute("type"));
		serviceLogger.debug("        -> dsid: " + e.getAttribute("dsid"));
		dsBean.setDataSetType(e.getAttribute("type"));

		NodeList datasetnl = e.getChildNodes();

		Element auxe = null;
		for (int i = 0; i < datasetnl.getLength(); i++) {
			if (datasetnl.item(i).getNodeType() == Node.ELEMENT_NODE) {
				auxe = (Element) datasetnl.item(i);
				if (datasetnl.item(i).getNodeName().toLowerCase()
						.equals("transexecutable")) {
					dsBean.setTransExecutableName(auxe.getAttribute("name"));
					dsBean.setTransExecutableDir(auxe.getAttribute("dir"));
				} else if (datasetnl.item(i).getNodeName().toLowerCase()
						.equals("sourcefile")) {
					dsBean.setSourceFileExtension(auxe
							.getAttribute("extension"));
					dsBean.setSourceFileLocation(auxe
							.getAttribute("where_is_located"));
					dsBean.setSourceFileRelationWithSon(auxe
							.getAttribute("relation_with_son"));
				} else if (datasetnl.item(i).getNodeName().toLowerCase()
						.equals("translateddataset")) {
					dsBean.setTranslatedDatasetType(auxe.getAttribute("type"));
					dsBean.setTranslatedDatasetRelationWithIR(auxe
							.getAttribute("relation_with_ir"));

					serviceLogger
							.debug("\n\nPRIONConfigManager -> relation_with_ir: "
									+ auxe.getAttribute("relation_with_ir"));

					NodeList transDsnl = auxe.getChildNodes();
					serviceLogger
							.debug("PRIONConfigManager -> NUM OF CHILD NODES: "
									+ auxe.getChildNodes().getLength());
					for (int x = 0; x < transDsnl.getLength(); x++) {
						if (transDsnl.item(x).getNodeType() == Node.ELEMENT_NODE) {
							if (transDsnl.item(x).getNodeName().toLowerCase()
									.equals("dsname")) {
								if (transDsnl.item(x).getFirstChild() != null) {
									dsBean.setDsName(transDsnl.item(x)
											.getFirstChild().getNodeValue());
									serviceLogger.debug("      dsname: "
											+ ((Element) transDsnl.item(x))
													.getFirstChild()
													.getNodeValue());
								}
							} else if (transDsnl.item(x).getNodeName()
									.toLowerCase().equals("dsnameprefix")) {
								if (transDsnl.item(x).getFirstChild() != null) {
									dsBean.setDsNamePrefix(((Element) transDsnl
											.item(x)).getFirstChild()
											.getNodeValue());
									serviceLogger.debug("      dsnameprefix: "
											+ ((Element) transDsnl.item(x))
													.getFirstChild()
													.getNodeValue());
								}
							} else if (transDsnl.item(x).getNodeName()
									.toLowerCase().equals("dsnamesufix")) {
								if (transDsnl.item(x).getFirstChild() != null) {
									dsBean.setDsNameSufix(((Element) transDsnl
											.item(x)).getFirstChild()
											.getNodeValue());
									serviceLogger.debug("      dsnamesufix: "
											+ ((Element) transDsnl.item(x))
													.getFirstChild()
													.getNodeValue());
								}
							} else if (transDsnl.item(x).getNodeName()
									.toLowerCase().equals("filename")) {
								if (transDsnl.item(x).getFirstChild() != null) {
									dsBean.setFileName(transDsnl.item(x)
											.getFirstChild().getNodeValue());
									serviceLogger.debug("      filename: "
											+ ((Element) transDsnl.item(x))
													.getFirstChild()
													.getNodeValue());
								}
							} else if (transDsnl.item(x).getNodeName()
									.toLowerCase().equals("filenameprefix")) {
								if (transDsnl.item(x).getFirstChild() != null) {
									dsBean.setFileNamePrefix(((Element) transDsnl
											.item(x)).getFirstChild()
											.getNodeValue());
									serviceLogger
											.debug("      filenameprefix: "
													+ ((Element) transDsnl
															.item(x))
															.getFirstChild()
															.getNodeValue());
								}
							} else if (transDsnl.item(x).getNodeName()
									.toLowerCase().equals("filenamesufix")) {
								if (transDsnl.item(x).getFirstChild() != null) {
									dsBean.setFileNameSufix(((Element) transDsnl
											.item(x)).getFirstChild()
											.getNodeValue());
									serviceLogger.debug("      filenamesufix: "
											+ ((Element) transDsnl.item(x))
													.getFirstChild()
													.getNodeValue());
								}
							} else if (transDsnl.item(x).getNodeName()
									.toLowerCase().equals("fileextension")) {
								if (transDsnl.item(x).getFirstChild() != null) {
									dsBean.setTranslatedFileExtension(((Element) transDsnl
											.item(x)).getFirstChild()
											.getNodeValue());
									serviceLogger.debug("      fileextension: "
											+ ((Element) transDsnl.item(x))
													.getFirstChild()
													.getNodeValue());
								}
							} else if (transDsnl.item(x).getNodeName()
									.toLowerCase().equals("namedreferencetype")) {
								if (transDsnl.item(x).getFirstChild() != null) {
									dsBean.setTranslatedFileReferenceType(((Element) transDsnl
											.item(x)).getFirstChild()
											.getNodeValue());
									serviceLogger
											.debug("      namedreferencetype: "
													+ ((Element) transDsnl
															.item(x))
															.getFirstChild()
															.getNodeValue());
								}
							} else {
								serviceLogger
										.debug("\n\n\n\nPRIONConfigManager -> NODE NAME DOESNT MATCH!!!: "
												+ transDsnl.item(x)
														.getNodeName()
														.toLowerCase()
												+ "\n\n\n\n");
							}
						}
					}
				}
			}

			/*
			 * serviceLogger.debug("PRIONConfigManager -> dsBean fulfilled!!");
			 * serviceLogger.debug("\t     -> getDataSetType: " +
			 * dsBean.getDataSetType());
			 * serviceLogger.debug("\t     -> getDataSetTypeID: " +
			 * dsBean.getDataSetTypeID());
			 * serviceLogger.debug("\t     -> getDsName: " +
			 * dsBean.getDsName());
			 * serviceLogger.debug("\t     -> getDsNamePrefix: " +
			 * dsBean.getDsNamePrefix());
			 * serviceLogger.debug("\t     -> getDsNameSufix: " +
			 * dsBean.getDsNameSufix());
			 * serviceLogger.debug("\t     -> getSourceFileExtension: " +
			 * dsBean.getSourceFileExtension());
			 * serviceLogger.debug("\t     -> getSourceFileLocation: " +
			 * dsBean.getSourceFileLocation());
			 * serviceLogger.debug("\t     -> getSourceFileRelationWithSon: " +
			 * dsBean.getSourceFileRelationWithSon());
			 * serviceLogger.debug("\t     -> getTransExecutableDir: " +
			 * dsBean.getTransExecutableDir());
			 * serviceLogger.debug("\t     -> getTransExecutableName: " +
			 * dsBean.getTransExecutableName());
			 * serviceLogger.debug("\t     -> getTranslatedDatasetRelationWithIR: "
			 * + dsBean.getTranslatedDatasetRelationWithIR());
			 * serviceLogger.debug("\t     -> getTranslatedDatasetType: " +
			 * dsBean.getTranslatedDatasetType());
			 * serviceLogger.debug("\t     -> getTranslatedFileExtension: " +
			 * dsBean.getTranslatedFileExtension());
			 * serviceLogger.debug("\t     -> getTranslatedFileReferenceType: "
			 * + dsBean.getTranslatedFileReferenceType());
			 */
		}
	}

	static boolean isPrionTranslator(String serviceName, Node myNode) {

		boolean isPrion = false;

		NodeList nl = myNode.getChildNodes();

		for (int i = 0; i < nl.getLength(); i++) {
			if (nl.item(i).getNodeName().toLowerCase()
					.equals(serviceName.toLowerCase() + "options")) {
				isPrion = true;
				prionOptionsNode = (Element) nl.item(i);
				break;
			}
		}

		return isPrion;
	}

	public static void readPRIONTaskConfiguration(
			PRIONTaskConfBean taskConfBean, String taskConfFilePath) {
		try {
			// get the factory
			DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
			// Using factory get an instance of document builder
			DocumentBuilder db = dbf.newDocumentBuilder();
			// parse using builder to get DOM representation of the XML file
			Document taskConfDOM = db.parse(new File(taskConfFilePath));
			// get the root element
			Element docEle = taskConfDOM.getDocumentElement();
			// get a nodelist of elements
			NodeList nl = docEle.getChildNodes();

			Element confElement = null;

			if (nl != null && nl.getLength() > 0) {
				for (int x = 0; x < nl.getLength(); x++) {
					if (nl.item(x).getNodeType() == Node.ELEMENT_NODE) {

						confElement = (Element) nl.item(x);

						if (confElement.getAttribute("type").toLowerCase()
								.equals("taskid")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setTaskID(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      taskid: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						} else if (confElement.getAttribute("type")
								.toLowerCase().equals("transexec")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setTransExec(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      transexecdir: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						} else if (confElement.getAttribute("type")
								.toLowerCase().equals("transexecdir")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setTransExecDir(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      transexecdir: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						} else if (confElement.getAttribute("type")
								.toLowerCase().equals("datasetname")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setDatasetName(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      datasetname: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						} else if (confElement.getAttribute("type")
								.toLowerCase().equals("filename")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setFileName(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      filename: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						} else if (confElement.getAttribute("type")
								.toLowerCase().equals("itemid")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setItemId(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      filename: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						} else if (confElement.getAttribute("type")
								.toLowerCase().equals("itemrevid")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setItemRevId(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      filename: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						} else if (confElement.getAttribute("type")
								.toLowerCase().equals("fileuid")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setFileUID(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      filename: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						} else if (confElement.getAttribute("type")
								.toLowerCase().equals("cadimentryuid")) {
							if (confElement.getFirstChild() != null) {
								taskConfBean.setCadimEntryUID(confElement
										.getFirstChild().getNodeValue());
								// serviceLogger.debug("      filename: " +
								// (confElement.getFirstChild().getNodeValue()));
							}
						}
					}
				}
			}
		} catch (SAXException ex) {
			Logger.getLogger(PRIONConfigManager.class.getName()).log(
					Level.SEVERE, null, ex);
		} catch (IOException ex) {
			Logger.getLogger(PRIONConfigManager.class.getName()).log(
					Level.SEVERE, null, ex);
		} catch (ParserConfigurationException ex) {
			Logger.getLogger(PRIONConfigManager.class.getName()).log(
					Level.SEVERE, null, ex);
		}
	}

	public static void writePRIONTaskConfiguration(
			boolean sanitized,
			ModelObject request,
			com.teamcenter.soa.client.model.strong.Dataset transPrimaryObject_dataset,
			ItemRevision transSecondaryObject_itemrev, String stagingLoc,
			String fileUID, String cadimEntryUID, String sourceObjectName,
			String sourceFileName, PRIONDSPDatasetBean x) {
		try {
			TRANSLATED_OBJECT_NAME = sourceObjectName;
			SELECTED_OBJECT_NAME = transPrimaryObject_dataset
					.get_current_name();
			FILE_UID = fileUID;
			CADIM_ENTRY_UID = cadimEntryUID;

			// Change file name if needed
			if (!sanitized)
				x.setFinalFileName(setFileName(sourceFileName, x));
			else
				x.setFinalFileName(sourceFileName);

			// Write configuration file
			FileOutputStream fos = null;
			try {
				Element e = null;
				Node n = null;
				// Document (Xerces implementation only).
				Document xmldoc = new DocumentImpl();
				// Root element.
				Element root = xmldoc.createElement("PRIONTask");
				String[] id = { "TASKID", "TRANSExec", "TRANSExecDir",
						"DATASETName", "FILEName", "FileUID", "CadimEntryUID" };
				// Changed for upgrade
				String[] value = {
						request.getPropertyObject(TranslationRequest.TASK_ID)
								.getDisplayableValue(),
						x.getTransExecutableName(), x.getTransExecutableDir(),
						setDSName(sourceObjectName, x), x.getFinalFileName(),
						FILE_UID, CADIM_ENTRY_UID };
				for (int i = 0; i < id.length; i++) {
					// Child i.
					e = xmldoc.createElementNS(null, "CONF");
					e.setAttributeNS(null, "type", id[i]);
					n = xmldoc.createTextNode(value[i]);
					e.appendChild(n);
					root.appendChild(e);
				}
				xmldoc.appendChild(root);
				fos = new FileOutputStream(stagingLoc + "\\PRIONTaskConf.xml");
				// XERCES 1 or 2 additionnal classes.
				OutputFormat of = new OutputFormat("XML", "ISO-8859-1", true);
				of.setIndent(1);
				of.setIndenting(true);
				XMLSerializer serializer = new XMLSerializer(fos, of);
				// As a DOM Serializer
				serializer.asDOMSerializer();
				serializer.serialize(xmldoc.getDocumentElement());
				fos.close();
			} catch (NotLoadedException ex) {
				Logger.getLogger(PRIONConfigManager.class.getName()).log(
						Level.SEVERE, null, ex);
			} catch (IOException ex) {
				Logger.getLogger(PRIONConfigManager.class.getName()).log(
						Level.SEVERE, null, ex);
			} finally {
				try {
					fos.close();
				} catch (IOException ex) {
					Logger.getLogger(PRIONConfigManager.class.getName()).log(
							Level.SEVERE, null, ex);
				}
			}
		} catch (NotLoadedException ex) {
			Logger.getLogger(PRIONConfigManager.class.getName()).log(
					Level.SEVERE, null, ex);
		}
	}

	public static void writePRIONTaskConfigurationNONE(ModelObject request,
			String stagingLoc, String fileUID, String cadimEntryUID,
			PRIONDSPDatasetBean x) {

		FILE_UID = fileUID;
		CADIM_ENTRY_UID = cadimEntryUID;

		// Write configuration file
		FileOutputStream fos = null;
		try {
			Element e = null;
			Node n = null;
			// Document (Xerces implementation only).
			Document xmldoc = new DocumentImpl();
			// Root element.
			Element root = xmldoc.createElement("PRIONTask");
			String[] id = { "TASKID", "TRANSExec", "TRANSExecDir",
					"DATASETName", "FILEName", "FileUID", "CadimEntryUID" };
			// Changed for upgrade
			String[] value = {
					request.getPropertyObject(TranslationRequest.TASK_ID)
							.getDisplayableValue(), x.getTransExecutableName(),
					x.getTransExecutableDir(), "NONE", "NONE", FILE_UID,
					CADIM_ENTRY_UID };
			for (int i = 0; i < id.length; i++) {
				// Child i.
				e = xmldoc.createElementNS(null, "CONF");
				e.setAttributeNS(null, "type", id[i]);
				n = xmldoc.createTextNode(value[i]);
				e.appendChild(n);
				root.appendChild(e);
			}
			xmldoc.appendChild(root);
			fos = new FileOutputStream(stagingLoc + "\\PRIONTaskConf.xml");
			// XERCES 1 or 2 additionnal classes.
			OutputFormat of = new OutputFormat("XML", "ISO-8859-1", true);
			of.setIndent(1);
			of.setIndenting(true);
			XMLSerializer serializer = new XMLSerializer(fos, of);
			// As a DOM Serializer
			serializer.asDOMSerializer();
			serializer.serialize(xmldoc.getDocumentElement());
			fos.close();
		} catch (NotLoadedException ex) {
			Logger.getLogger(PRIONConfigManager.class.getName()).log(
					Level.SEVERE, null, ex);
		} catch (IOException ex) {
			Logger.getLogger(PRIONConfigManager.class.getName()).log(
					Level.SEVERE, null, ex);
		} finally {
			try {
				fos.close();
			} catch (IOException ex) {
				Logger.getLogger(PRIONConfigManager.class.getName()).log(
						Level.SEVERE, null, ex);
			}
		}
	}

	private static String setDSName(String sourceObjectName,
			PRIONDSPDatasetBean x) {
		String datasetName = sourceObjectName;

		if (!x.getDsName().isEmpty()) {
			serviceLogger.debug("\tPRIONConfigManager -> getDsName(): "
					+ x.getDsName());
			datasetName = translateVariablesLiterals(x.getDsName());
		}

		if (!x.getDsNamePrefix().isEmpty()) {
			serviceLogger.debug("\tPRIONConfigManager -> getDsNamePrefix(): "
					+ x.getDsNamePrefix());
			datasetName = translateVariablesLiterals(x.getDsNamePrefix()
					+ datasetName);
		}

		if (!x.getDsNameSufix().isEmpty()) {
			serviceLogger.debug("\tPRIONConfigManager -> getDsNameSufix(): "
					+ x.getDsNameSufix());
			datasetName = datasetName
					+ translateVariablesLiterals(x.getDsNameSufix());
		}

		return datasetName;
	}

	private static String setFileName(String sourceFileName,
			PRIONDSPDatasetBean x) {
		String fileName = sourceFileName;

		if (!x.getFileName().isEmpty()) {
			serviceLogger.debug("\tPRIONConfigManager -> getFileName(): "
					+ x.getFileName());
			fileName = translateVariablesLiterals(x.getFileName());
		}

		if (!x.getFileNamePrefix().isEmpty()) {
			serviceLogger.debug("\tPRIONConfigManager -> getFileNamePrefix(): "
					+ x.getFileNamePrefix());
			fileName = translateVariablesLiterals(x.getFileNamePrefix())
					+ fileName;
		}

		if (!x.getDsNameSufix().isEmpty()) {
			serviceLogger.debug("\tPRIONConfigManager -> getFileNameSufix(): "
					+ x.getFileNameSufix());
			fileName = fileName
					+ translateVariablesLiterals(x.getFileNameSufix());
		}

		return fileName;
	}

	private static String translateVariablesLiterals(String str) {
		String finalString = "";

		serviceLogger.debug("\tPRIONConfigManager -> Name String: " + str);

		String fieldsArray[] = str.split("-PrioN-");

		for (int i = 1; i < fieldsArray.length; i++) {
			if (fieldsArray[i].startsWith("$")) {
				finalString = finalString + getVariableValue(fieldsArray[i]);
			} else {
				finalString = finalString + fieldsArray[i];
			}
		}

		return finalString;
	}

	private static String getVariableValue(String x) {
		if (x.equals("$TRANSLATED_OBJECT_NAME")) {
			return TRANSLATED_OBJECT_NAME;
		} else if (x.equals("$SELECTED_OBJECT_NAME")) {
			return SELECTED_OBJECT_NAME;
		} else if (x.equals("$ITEM_ID")) {
			return "?item_id?";
		} else if (x.equals("$ITEM_REV_ID")) {
			return "?item_rev_id?";
		} else {
			return "PRIONTranslator_NAME_ERROR";
		}
	}
}
