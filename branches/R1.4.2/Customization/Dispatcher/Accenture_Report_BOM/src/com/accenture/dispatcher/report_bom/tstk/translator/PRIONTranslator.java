/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.accenture.dispatcher.report_bom.tstk.translator;

/**
 *
 * @author ftorres based on SIEMESN DefaultTranslator class
 */
import com.accenture.dispatcher.report_bom.ets.util.PRIONConfigManager;
import com.accenture.dispatcher.report_bom.ets.util.PRIONTaskConfBean;
import com.teamcenter.tstk.server.scheduler.ScheduleTask;
import com.teamcenter.tstk.util.log.LoggerFactoryUtil;
import com.teamcenter.tstk.translator.*;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Map;
import java.util.Properties;
import java.util.Random;
import java.util.Vector;
import javax.xml.parsers.ParserConfigurationException;

@SuppressWarnings("serial")
public class PRIONTranslator extends AbstractTranslator implements BaseTranslatorConstants {

	private Random m_zRandom;
	private DefaultFilenameFilter m_zFilenameFilter;

	public PRIONTranslator(String scTag) throws TranslatorException {
		this.m_zProcessLogger = LoggerFactoryUtil.getProcessLogger(LoggerFactoryUtil.getMainLoggerName());

		this.m_zProcessLogger.trace("PRIONTranslator -> DefaultTranslator::constructor", 0);
		try {
			this.m_zRandom = new Random();
			setTranslatorData(scTag);
			this.m_zProcessLogger.debug("PRIONTranslator -> Translator data populated. ");

			if (this.m_scDaemonCommand != null) {
				this.m_zProcessLogger.info("PRIONTranslator -> Starting Daemon Service for Provider: " + this.m_scProvider + " Service: "
						+ this.m_scService + " Command: " + this.m_scDaemonCommand);
				try {
					Runtime.getRuntime().exec(this.m_scDaemonCommand);
				} catch (Exception zExcept) {
					this.m_zProcessLogger.error("PRIONTranslator -> An Exception was thrown while trying to run the Daemon Service ", zExcept);
				}

			}

			this.m_zFilenameFilter = new DefaultFilenameFilter();
		} catch (ParserConfigurationException zExcept) {
			String scMessage = "ParserConfigurationException while reading translator.xml file";

			this.m_zProcessLogger.error(scMessage, zExcept);

			throw new TranslatorException(scMessage + " " + zExcept.getMessage());
		} catch (IOException zExcept) {
			String scMessage = "PRIONTranslator -> IOException while reading translator.xml file";

			this.m_zProcessLogger.error(scMessage, zExcept);

			throw new TranslatorException(scMessage + " " + zExcept.getMessage());
		}
	}

	public TranslatorRunnable getRunnable(TranslatorListener zTranslatorListener, TranslatorTask zTranslatorTask, TranslatorMap zTranslatorMap)
			throws TranslatorException {
		return new DefaultTranslatorRunnable(zTranslatorListener, zTranslatorTask, zTranslatorMap);
	}

	protected void setTranslatorData(String scTransTag) throws ParserConfigurationException, IOException {
		BaseTranslatorParser.setTranslatorData(this, scTransTag);
	}

	private class DefaultFilenameFilter implements FilenameFilter {

		private DefaultFilenameFilter() {
		}

		@Override
		public boolean accept(File zFile, String scName) {
			if (PRIONTranslator.this.m_vOutputExt != null) {
				int nExt = PRIONTranslator.this.m_vOutputExt.size();
				for (int i = 0; i < nExt; i++) {
					String scExt = (String) PRIONTranslator.this.m_vOutputExt.get(i);

					if (scName.toLowerCase().endsWith(scExt) == true) {
						return true;
					}
				}
				return false;
			}

			return true;
		}
	}

	protected class DefaultTranslatorRunnable extends AbstractTranslator.AbstractTranslatorRunnable {

		protected String m_scDir_Src;

		protected DefaultTranslatorRunnable(TranslatorListener zTranslatorListener, TranslatorTask zTranslatorTask, TranslatorMap zTranslatorMap) {
			super(zTranslatorListener, zTranslatorTask, zTranslatorMap);

			this.m_zTaskLogger.trace("PRIONTranslator -> DefaultTranslator$DefaultTranslatorRunnable::constructor", 0);
		}

		public void startTranslationProcess(Vector<String> vTransCmd) throws IOException {

			PRIONTaskConfBean taskConf = new PRIONTaskConfBean();

			this.m_zTaskLogger.info("PRIONTranslator -> " + m_scDir_Src);
			PRIONConfigManager.readPRIONTaskConfiguration(taskConf, m_scDir_Src + "\\PRIONTaskConf.xml");
			this.m_zTaskLogger.info("PRIONTranslator ->    DSName: " + taskConf.getDatasetName());
			this.m_zTaskLogger.info("PRIONTranslator ->    FileName: " + taskConf.getFileName());
			this.m_zTaskLogger.info("PRIONTranslator ->    TransEXEC: " + m_scTranslatorExecutable); // taskConf.getTransExec());
			this.m_zTaskLogger.info("PRIONTranslator ->    TEDir: " + taskConf.getTransExecDir());
			this.m_zTaskLogger.info("PRIONTranslator ->    TaskID: " + taskConf.getTraskID());
			this.m_zTaskLogger.info("PRIONTranslator ->    ItemId: " + taskConf.getItemId());
			this.m_zTaskLogger.info("PRIONTranslator ->    ItemRevId: " + taskConf.getItemRevId());
			this.m_zTaskLogger.info("PRIONTranslator ->    ReportType: " + taskConf.getReportType());
			this.m_zTaskLogger.info("PRIONTranslator ->    RevisionRule: " + taskConf.getRevisionRule());
			this.m_zTaskLogger.info("PRIONTranslator ->    ReportFormat: " + taskConf.getReportFormat());
			this.m_zTaskLogger.info("PRIONTranslator ->    UserName: " + taskConf.getUserName());
			this.m_zTaskLogger.info("PRIONTranslator ->    StagingLoc: " + taskConf.getStagingLoc());
			this.m_zTaskLogger.info("PRIONTranslator ->    MaxBOMLevel: " + taskConf.getMaxBOMLevel());
			this.m_zTaskLogger.info("PRIONTranslator ->    EqualPart: " + taskConf.getEqualPart());
			this.m_zTaskLogger.info("PRIONTranslator ->    Ordering: " + taskConf.getOrdering());
			this.m_zTaskLogger.info("PRIONTranslator ->    Included PCB: " + taskConf.getIncludePCB());
			this.m_zTaskLogger.info("PRIONTranslator ->    Included SUP: " + taskConf.getIncludeSUP());
			this.m_zTaskLogger.info("PRIONTranslator ->    Only SMD: " + taskConf.getOnlySMD());
			this.m_zTaskLogger.info("PRIONTranslator ->    ReportFilename: " + taskConf.getReportFilename());

			addMultipleConfigs(vTransCmd);

			waitTillNotified();

			synchronized (this) {
				if (this.m_qStop) {
					return;
				}
				int iSize = vTransCmd.size();
				String scFinalCmd = "PRIONTranslator -> Command string:\n";

				String[] ascCommand = new String[iSize];

				if (taskConf.getDatasetName().equals("NONE") && taskConf.getFileName().equals("NONE")) {

					String[] ascCommandNONE = new String[15];

					ascCommandNONE[0] = m_scTranslatorExecutable;// taskConf.getTransExecDir()
																	// +
																	// File.separator
																	// +
																	// taskConf.getTransExec();
					ascCommandNONE[1] = "\"" + taskConf.getItemId() + "\"";
					ascCommandNONE[2] = taskConf.getItemRevId();
					ascCommandNONE[3] = taskConf.getReportType();
					ascCommandNONE[4] = taskConf.getRevisionRule();
					ascCommandNONE[5] = taskConf.getReportFormat();
					ascCommandNONE[6] = taskConf.getUserName();
					ascCommandNONE[7] = taskConf.getStagingLoc();
					ascCommandNONE[8] = taskConf.getMaxBOMLevel();
					ascCommandNONE[9] = taskConf.getEqualPart();
					ascCommandNONE[10] = taskConf.getOrdering();
					ascCommandNONE[11] = taskConf.getIncludePCB();
					ascCommandNONE[12] = taskConf.getIncludeSUP();
					ascCommandNONE[13] = taskConf.getOnlySMD();
					ascCommandNONE[14] = taskConf.getReportFilename();					
					
					this.m_zTaskLogger.info(scFinalCmd + "\n");
					this.m_zMainProcess = Runtime.getRuntime().exec(ascCommandNONE, null, new File(this.m_scDir_Src));

				} else {
					for (int i = 0; i < iSize; i++) {
						ascCommand[i] = vTransCmd.get(i);
						if (i == 0) {
							// The first argument is the translator executable.
							// We will change the default one by our own if
							// the user has specified an specific translator
							// executable for this task
							if (!taskConf.getTransExec().isEmpty()) {
								ascCommand[i] = taskConf.getTransExecDir() + File.separator + taskConf.getTransExec();
								scFinalCmd = scFinalCmd + taskConf.getTransExecDir() + File.separator + taskConf.getTransExec() + ' ';
							} else {
								scFinalCmd = scFinalCmd + (String) vTransCmd.get(i) + ' ';
							}
						} else {
							scFinalCmd = scFinalCmd + (String) vTransCmd.get(i) + ' ';
						}
						this.m_zTaskLogger.info("PRIONTranslator -> ARG " + i + ": " + (String) vTransCmd.get(i));
					}

					this.m_zTaskLogger.info(scFinalCmd + "\n");
					this.m_zMainProcess = Runtime.getRuntime().exec(ascCommand, null, new File(this.m_scDir_Src));
				}

			}
		}

		@SuppressWarnings("rawtypes")
		public void addMultipleConfigs(Vector<String> vTransCmd) throws IOException {
			int iMapIndex = vTransCmd.indexOf("-tstkmap");
			if (iMapIndex == -1) {
				return;
			}

			String scMapFile = vTransCmd.remove(iMapIndex + 1);

			this.m_zTaskLogger.debug("\n\n\n\n\n\n\n\nPRIONTranslator -> iMAPIdex: " + (String) vTransCmd.remove(iMapIndex + 1));

			vTransCmd.remove(iMapIndex);

			File zMapFile = new File(scMapFile);
			if (!zMapFile.exists()) {
				throw new IOException("PRIONTranslator -> Property file does not exist, expecting " + scMapFile);
			}

			Properties zMapProp = new Properties();
			zMapProp.load(new FileInputStream(zMapFile));

			Enumeration zMapKeyEnum = zMapProp.keys();

			Map zOptions = this.m_zTranslatorTask.getOptions();
			if (zOptions == null) {
				throw new IOException("PRIONTranslator -> Client Translator Options are not passed.");
			}

			boolean qFound = false;
			while (zMapKeyEnum.hasMoreElements()) {
				String scKey = (String) zMapKeyEnum.nextElement();

				if (zOptions.containsValue(scKey)) {
					String scArg = (String) zMapProp.get(scKey);
					vTransCmd.add(iMapIndex, scArg);
					iMapIndex++;
					qFound = true;
				}
			}
			if (!qFound) {
				throw new IOException("PRIONTranslator -> Client Translator Options do not match the Property keys in file: " + scMapFile);
			}

			String scCmd = "PRIONTranslator -> Command String with client options and Multiple config options: ";

			for (int i = 0; i < vTransCmd.size(); i++) {
				scCmd = scCmd + vTransCmd.get(i);
				scCmd = scCmd + " ";
			}
			this.m_zTaskLogger.debug(scCmd);
		}

		@Override
		@SuppressWarnings({ "unused", "rawtypes", "unchecked" })
		public TranslatorEvent translate(String scTaskId, String scDir_Src, String scFile_Src, String scDir_Dst, String scFile_Dst) {
			TranslatorEvent zEvent = null;
			boolean qSrcDirChg = false;
			String scOrgDir_Src = null;

			this.m_zTaskLogger.debug("PRIONTranslator -> Started translation process........");

			String scSrcPath = scDir_Src + File.separatorChar + scFile_Src;

			this.m_zTaskLogger.debug("PRIONTranslator -> Source file for translation = " + scSrcPath);
			String scDir_Random;
			if ((PRIONTranslator.this.m_iSrcPathLimit != -1) && (scSrcPath.length() > PRIONTranslator.this.m_iSrcPathLimit)) {
				qSrcDirChg = true;

				scDir_Random = PRIONTranslator.this.getWorkingDir() + File.separatorChar
						+ Integer.toHexString(PRIONTranslator.this.m_zRandom.nextInt());

				this.m_zTaskLogger.debug("PRIONTranslator -> Source file being moved from [ " + scDir_Src + " ] to [ " + scDir_Random + " ]");

				moveDirContents(scDir_Src, scDir_Random);

				scOrgDir_Src = scDir_Src;

				scDir_Src = scDir_Random;
			}
			this.m_scTaskId = scTaskId;
			try {
				scDir_Src = new File(scDir_Src).getCanonicalPath();
				scDir_Dst = new File(scDir_Dst).getCanonicalPath();

				this.m_zTaskLogger.debug("PRIONTranslator -> Source directory " + scDir_Src + " Destination directory " + scDir_Dst);

				if (PRIONTranslator.this.isPreProcessNeeded()) {
					this.m_zTaskLogger.debug("PRIONTranslator -> Started preprocess " + PRIONTranslator.this.m_scPrePsService + " Id: " + scTaskId);

					((ScheduleTask) this.m_zTranslatorTask).setStatus(6);

					zEvent = preProcess(scTaskId, scDir_Src, scFile_Src, scDir_Src, null, PRIONTranslator.this.m_scPrePsProvider,
							PRIONTranslator.this.m_scPrePsService);

					if (zEvent.getStatus() != 3) {
						scDir_Random = zEvent.getMessage();
						// jsr 1581;
					}

					((ScheduleTask) this.m_zTranslatorTask).setStatus(8);

					scFile_Src = (String) zEvent.getOutputFileNames().get(0);

					this.m_zTaskLogger.debug("PRIONTranslator -> Source file after pre-procesing = " + scFile_Src);
				}

				this.m_zTaskLogger.debug("PRIONTranslator -> ");
				this.m_zTaskLogger.debug("\t Value of scDir_Src: " + scDir_Src);
				this.m_zTaskLogger.debug("\t Value of scFile_Src: " + scFile_Src);
				this.m_zTaskLogger.debug("\t Value of scDir_Dst: " + scDir_Dst);
				this.m_zTaskLogger.debug("\t Value of scFile_Dst: " + scFile_Dst);

				// We will obtain the output file name from the Task XML info
				// file.
				Element xmlElement = parseXMLFile(scDir_Src + "\\" + scDir_Src.substring((scDir_Src.lastIndexOf("\\")) + 1) + ".xml");

				String scFileDst_Chg = null;
				if (scFile_Dst == null) {
					scFile_Dst = "NONE";
					scFileDst_Chg = scFile_Dst;
				} else {
					scFileDst_Chg = validateFileNames(scDir_Src, scFile_Src, scDir_Dst, scFile_Dst);
					this.m_zTaskLogger.debug("\t Original Dest file : " + scFile_Dst);
					scFileDst_Chg = xmlElement.getAttribute("name");
					scFile_Dst = scFileDst_Chg;
					this.m_zTaskLogger.debug("\t Changed Dest file : " + scFileDst_Chg);
				}

				Vector vCmds;

				vCmds = PRIONTranslator.this.getCommandArray(this.m_zTranslatorTask, scDir_Src, this.m_scFile_In, scDir_Dst, scFileDst_Chg);

				Vector vCmds_backup = new Vector(vCmds);

				this.m_scDir_Src = scDir_Src;

				String scTransLogFile = null;

				if ((PRIONTranslator.this.m_scTransLogFile != null) && (PRIONTranslator.this.m_scTransLogFile.length() > 0)) {
					scTransLogFile = scDir_Dst + File.separatorChar + PRIONTranslator.this.m_scTransLogFile;
				}

				for (int i = 0; i < PRIONTranslator.this.m_iReTries; i++) {
					startTranslationProcess(vCmds);

					this.m_zTaskLogger.debug("PRIONTranslator -> Started translation " + PRIONTranslator.this.getService() + " Id: " + scTaskId);

					this.m_zTaskLogger.debug("PRIONTranslator ->  Trying " + (i + 1) + " times out of " + PRIONTranslator.this.m_iReTries);

					BaseMessageHandler zBaseMsgHandler = new BaseMessageHandler();

					int iTaskStatus = zBaseMsgHandler.checkForErrors(PRIONTranslator.this.m_vINPUT, PRIONTranslator.this.m_vERROR,
							PRIONTranslator.this.m_vExclINPUT, PRIONTranslator.this.m_vExclERROR, PRIONTranslator.this.m_vExclExitVal,
							scTransLogFile, this.m_zMainProcess, this.m_zTaskLogger);
					TranslatorEvent localTranslatorEvent1;
					if (this.m_qStop) {
						zEvent = new BaseTranslatorEvent(scTaskId, 2, "INTERRUPTEDTASK.MSG");

						this.m_zTaskLogger.debug("PRIONTranslator -> The task " + scTaskId + " was interrupted. Translation Interrupted!");

						localTranslatorEvent1 = zEvent;
						// jsr 1121;
					}
					TranslatorEvent localTranslatorEvent2;
					int iStatus;
					TranslatorEvent localTranslatorEvent3;
					if (iTaskStatus == 1) {
						String scErrorMsg = "PRIONTranslator -> An error has been detected for task " + scTaskId
								+ " during the execution of Provider: " + PRIONTranslator.this.getProviderName() + ", Service: "
								+ PRIONTranslator.this.getService() + ". Refer to module task log (" + scTaskId
								+ LoggerFactoryUtil.getLogFileSuffix() + ") for further details.";

						this.m_zTaskLogger.debug(scErrorMsg);

						if (i == PRIONTranslator.this.m_iReTries - 1) {
							zEvent = new BaseTranslatorEvent(scTaskId, 1, scErrorMsg);

							localTranslatorEvent2 = zEvent;
							// jsr 996;
						}
					} else if (iTaskStatus == 3) {
						synchronized (this) {
							this.m_zMainProcess = null;
						}
						this.m_zTaskLogger.info("PRIONTranslator -> Task = " + scTaskId + ", Status = " + "The translator completed successfully");

						File[] ascDstFile = new File(scDir_Dst).listFiles(PRIONTranslator.this.m_zFilenameFilter);

						if ((PRIONTranslator.this.m_qOutputNeeded == true) && ((ascDstFile == null) || (ascDstFile.length == 0))) {
							this.m_zTaskLogger.error("PRIONTranslator -> No results found in result directory!");

							zEvent = new BaseTranslatorEvent(scTaskId, 1, "NORESULTSFILE.MSG");

							((ScheduleTask) this.m_zTranslatorTask).setStatus(13);

							localTranslatorEvent2 = zEvent;
							// jsr 834;
						}

						if (PRIONTranslator.this.isPostProcessOnDifferentMachine()) {
							this.m_zTranslatorTask.setPostprocessProvider(PRIONTranslator.this.m_scPostPsProvider);

							this.m_zTranslatorTask.setPostprocessService(PRIONTranslator.this.m_scPostPsService);

							this.m_zTaskLogger.debug("PRIONTranslator -> Post processor on a different machine Provider "
									+ PRIONTranslator.this.m_scPostPsProvider + " Service " + PRIONTranslator.this.m_scPostPsService);

							zEvent = new BaseTranslatorEvent(scTaskId, 5, "POSTPSDIFFMACHINE.MSG");

							zEvent.addOutputFileName(scFileDst_Chg);
							this.m_zTaskLogger.debug("File " + scFileDst_Chg + " added to list of output files");

							localTranslatorEvent2 = zEvent;
							// jsr 677;
						}
						String scFileList;
						if (PRIONTranslator.this.isPostProcessSupported()) {
							this.m_zTaskLogger.debug("PRIONTranslator -> Postprocess " + PRIONTranslator.this.m_scPostPsService + " Id: " + scTaskId);

							String[] ascFile = new File(scDir_Dst).list(PRIONTranslator.this.m_zFilenameFilter);

							if (ascFile.length == 0) {
								this.m_zTaskLogger.error("PRIONTranslator -> Error occured in post processing = POSTPROCESSFAILNOJT.EXC");

								throw new FileNotFoundException("POSTPROCESSFAILNOJT.EXC");
							}

							((ScheduleTask) this.m_zTranslatorTask).setStatus(19);

							zEvent = postProcess(scTaskId, scDir_Dst, ascFile[0], scDir_Dst, null, PRIONTranslator.this.m_scPostPsProvider,
									PRIONTranslator.this.m_scPostPsService);

							iStatus = zEvent.getStatus();

							if ((iStatus == 3) || (iStatus == 4)) {
								zEvent.addOutputFileName(scFileDst_Chg);

								scFileList = "";
								if (iStatus == 4) {
									((ScheduleTask) this.m_zTranslatorTask).setStatus(20);
									scFileList = "Post processing error occured and ";
								} else if (iStatus == 3) {
									((ScheduleTask) this.m_zTranslatorTask).setStatus(21);
									scFileList = "Post processing finished and ";
								}
								scFileList = scFileList + "file " + scFileDst_Chg + " added to the post processed list of files";

								this.m_zTaskLogger.debug(scFileList);
							}

							scFileList = zEvent.getMessage();
							// jsr 394;
						}

						zEvent = new BaseTranslatorEvent(scTaskId, 3, "COMPLETEDTASK.MSG");

						String scMainDstFile = getMainOutputFile(scDir_Dst);
						if (scMainDstFile != null) {
							scFileDst_Chg = scMainDstFile;
						}
						zEvent.addOutputFileName(scFileDst_Chg);
						this.m_zTaskLogger.debug(" File " + scFileDst_Chg + " added to list of output files");

						((ScheduleTask) this.m_zTranslatorTask).setStatus(15);

						localTranslatorEvent3 = zEvent;
						// jsr 297;
					}

					vCmds = vCmds_backup;
					endTranslationProcess();

					this.m_zTaskLogger.debug(" PRIONTranslator -> Waiting for " + PRIONTranslator.this.m_iWaitTimeForReTries
							+ " seconds before retry!");
					Thread.sleep(PRIONTranslator.this.m_iWaitTimeForReTries * 1000);
				}

				// i = zEvent;
				return zEvent;
			} catch (NullPointerException zExcept) {
				this.m_zTaskLogger.warn("PRIONTranslator -> NullPointerException occured during translation", zExcept);

				zEvent = new BaseTranslatorEvent(scTaskId, 1, zExcept.getMessage());

				return zEvent;
			} catch (FileNotFoundException zExcept) {
				this.m_zTaskLogger.warn("PRIONTranslator -> FileNotFoundException occured during translation", zExcept);

				zEvent = new BaseTranslatorEvent(scTaskId, 1, zExcept.getMessage());

				return zEvent;
			} catch (IOException zExcept) {
				this.m_zTaskLogger.warn("PRIONTranslator -> IOException occured during translation", zExcept);

				zEvent = new BaseTranslatorEvent(scTaskId, 1, zExcept.getMessage());

				return zEvent;
			} catch (InterruptedException zExcept) {
				Vector vCmds;
				this.m_zTaskLogger.warn("PRIONTranslator -> InterruptedException occured during translation", zExcept);

				zEvent = new BaseTranslatorEvent(scTaskId, 1, zExcept.getMessage());

				return zEvent;
			} finally {
				endTranslationProcess();

				if ((qSrcDirChg) && (zEvent != null)) {
					this.m_zTaskLogger.debug("PRIONTranslator -> Moving back contents from random dir...");
					moveDirContents(scDir_Src, scOrgDir_Src);
					zEvent.setDeleteDirectory(scDir_Src);
				}
				cleanup();

				this.m_zTaskLogger.debug("PRIONTranslator -> End translation " + PRIONTranslator.this.getService() + " Id: " + scTaskId);
			}
		}

		protected void moveDirContents(String scSrcDir, String scDestDir) {
			try {
				this.m_zTaskLogger.debug("PRIONTranslator -> Move files from directory " + scSrcDir + " to directory " + scDestDir);

				new File(scDestDir).mkdir();

				File[] azFile = new File(scSrcDir).listFiles();

				for (int i = 0; i < azFile.length; i++) {
					String scName = azFile[i].getName();

					if (azFile[i].isFile()) {
						this.m_zTaskLogger.debug("PRIONTranslator -> Move file " + azFile[i].toString() + " to " + scDestDir + File.separatorChar
								+ scName);

						azFile[i].renameTo(new File(scDestDir + File.separatorChar + scName));
					} else {
						moveDirContents(scSrcDir + File.separatorChar + scName, scDestDir + File.separatorChar + scName);
					}
				}

			} catch (Exception zEx) {
				this.m_zTaskLogger.debug("PRIONTranslator -> Move failed because of exception " + zEx.getMessage());
			}
		}

		@Override
		public String validateFileNames(String scDir_Src, String scFile_Src, String scDir_Dst, String scFile_Dst) {

			this.m_scFile_In = scFile_Src;

			// We will obtain the output file name from the Task XML info file.
			Element xmlElement = parseXMLFile(scDir_Src + "\\" + scDir_Src.substring((scDir_Src.lastIndexOf("\\")) + 1) + ".xml");
			this.m_zTaskLogger.debug("\t Task XML info file: " + scDir_Src + "\\" + scDir_Src.substring((scDir_Src.lastIndexOf("\\")) + 1) + ".xml");

			return xmlElement.getAttribute("name");
		}
	}

	public static Element parseXMLFile(String taskXMLFile) {
		NodeList nList = null;

		try {
			DocumentBuilderFactory docBuilderFactory = DocumentBuilderFactory.newInstance();
			docBuilderFactory.setIgnoringElementContentWhitespace(true);
			docBuilderFactory.setIgnoringComments(true);
			DocumentBuilder docBuilder = docBuilderFactory.newDocumentBuilder();
			Document doc = docBuilder.parse(new File(taskXMLFile));
			doc.getDocumentElement().normalize();

			nList = doc.getElementsByTagName("MappedFile");

		} catch (SAXException ex) {
			Logger.getLogger(PRIONTranslator.class.getName()).log(Level.SEVERE, null, ex);
		} catch (IOException ex) {
			Logger.getLogger(PRIONTranslator.class.getName()).log(Level.SEVERE, null, ex);
		} catch (ParserConfigurationException ex) {
			Logger.getLogger(PRIONTranslator.class.getName()).log(Level.SEVERE, null, ex);
		}

		return (Element) nList.item(0);
	}
}
